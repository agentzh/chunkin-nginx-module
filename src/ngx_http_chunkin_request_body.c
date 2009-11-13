#define DDEBUG 1

#include "ddebug.h"

#include "ngx_http_chunkin_util.h"
#include "chunked_parser.h"
#include "ngx_http_chunkin_filter_module.h"
#include "ngx_http_chunkin_request_body.h"

static ngx_int_t ngx_http_test_expect(ngx_http_request_t *r);

static void ngx_http_chunkin_read_chunked_request_body_handler(
        ngx_http_request_t *r);

static ngx_int_t ngx_http_chunkin_do_read_chunked_request_body(
        ngx_http_request_t *r);

static ngx_int_t ngx_http_write_request_body(ngx_http_request_t *r, ngx_chain_t *body);

/* this function's implementation is borrowed from nginx 0.8.20
 * and modified a bit to work with the chunked encoding.
 * Copyrighted (C) by Igor Sysoev */
ngx_int_t
ngx_http_chunkin_read_chunked_request_body(ngx_http_request_t *r,
    ngx_http_client_body_handler_pt post_handler)
{
    size_t                      preread;
    ssize_t                     size;
    ngx_http_request_body_t     *rb;
    ngx_http_core_loc_conf_t    *clcf;
    ngx_http_chunkin_ctx_t      *ctx;
    ngx_int_t                   rc;

    /* r->request_body_in_single_buf = 1; */
    /* r->request_body_in_file_only = 1; */

#if defined(nginx_version) && nginx_version >= 8011

    r->main->count++;

#endif

    if (r->request_body || r->discard_body) {
        post_handler(r);
        return NGX_OK;
    }

    if (ngx_http_test_expect(r) != NGX_OK) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    rb = ngx_pcalloc(r->pool, sizeof(ngx_http_request_body_t));
    if (rb == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    r->request_body = rb;

    clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);

    rb->post_handler = post_handler;

    ctx = ngx_http_get_module_ctx(r, ngx_http_chunkin_filter_module);
    if (ctx == NULL) {
        ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_chunkin_ctx_t));
        if (ctx == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        ngx_http_set_ctx(r, ctx, ngx_http_chunkin_filter_module);
    }

    preread = r->header_in->last - r->header_in->pos;

    ngx_http_chunkin_init_chunked_parser(r, ctx);

    if (preread) {
        ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                       "http chunked client request body preread %uz",
                       preread);

            dd("raw chunked body (len %d): %s", r->header_in->last - r->header_in->pos, r->header_in->pos);

        rc = ngx_http_chunkin_run_chunked_parser(r, ctx,
                r->header_in->pos, r->header_in->last);

        if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
            return rc;
        }

        if (rc == NGX_ERROR) {
            /* chunked body parsefail */
            return NGX_HTTP_BAD_REQUEST;
        }

        if (rc == NGX_OK) {
            /* TODO: take r->request_body_in_file_only
             *       into account */

            dd("we have the whole chunked body in 'prepread'...");

            if (ctx->chunks == NULL) {
                dd("zero-size chunk found.");
                return NGX_OK;
            }

            dd("chunks total size: %d", ctx->chunks_total_size);

            rc = ngx_http_chunkin_set_content_length_header(r, ctx->chunks_total_size);

            if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
                return rc;
            }

            rb->bufs = ctx->chunks;
            rb->buf = ctx->chunks->buf;

            dd("buf len: %d", rb->buf->last - rb->buf->pos);

            r->header_in->pos = r->header_in->last;

            r->request_length += preread;

            return NGX_OK;
        }

        if (rc != NGX_AGAIN) {
            /* this is impossible to reach... */
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        dd("we need to read more chunked body addition to 'prepread'...");

        r->header_in->pos = r->header_in->last;
        r->request_length += preread;
    }

    size = clcf->client_body_buffer_size;

    dd("chunks total size after preread: %d", ctx->chunks_total_size);

    rb->buf = ngx_create_temp_buf(r->pool, size);

    if (rb->buf == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    r->read_event_handler = ngx_http_chunkin_read_chunked_request_body_handler;

    return ngx_http_chunkin_do_read_chunked_request_body(r);
}

static void
ngx_http_chunkin_read_chunked_request_body_handler(ngx_http_request_t *r)
{
    ngx_int_t  rc;

    if (r->connection->read->timedout) {
        r->connection->timedout = 1;
        ngx_http_finalize_request(r, NGX_HTTP_REQUEST_TIME_OUT);
        return;
    }

    rc = ngx_http_chunkin_do_read_chunked_request_body(r);

    if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        ngx_http_finalize_request(r, rc);
    }
}

static ngx_int_t
ngx_http_chunkin_do_read_chunked_request_body(ngx_http_request_t *r)
{
    ngx_int_t                  rc;
    size_t                     size;
    ssize_t                    n;
    ngx_buf_t                 *b;
    ngx_connection_t          *c;
    ngx_http_request_body_t   *rb;
    ngx_http_core_loc_conf_t  *clcf;
    ngx_http_chunkin_ctx_t    *ctx;
    ngx_flag_t                 done;
    ngx_chain_t               *cl;

    c = r->connection;
    rb = r->request_body;
    ctx = ngx_http_get_module_ctx(r, ngx_http_chunkin_filter_module);

    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, c->log, 0,
                   "http chunkin read chunked client request body");

    dd("(outer) last == end ? %s",
            rb->buf->last == rb->buf->end ?
            "yes" : "no");

    done = 0;
    for ( ;; ) {
        for ( ;; ) {
            if (rb->buf->last == rb->buf->end) {
                if (ctx->chunks) {
                    rc = ngx_http_write_request_body(r, ctx->chunks);
                    if (rc != NGX_OK) {
                        return NGX_HTTP_INTERNAL_SERVER_ERROR;
                    }

                    ctx->chunk->buf->pos = rb->buf->start;
                    ctx->chunk->buf->start = rb->buf->start;
                    ctx->chunk->buf->end = rb->buf->start;
                    ctx->chunk->buf->last = rb->buf->start;

                    ctx->chunks = ctx->chunk;
                }

                rb->buf->last = rb->buf->start;
            }

            size = rb->buf->end - rb->buf->last;

            n = c->recv(c, rb->buf->last, size);
            ngx_log_debug1(NGX_LOG_DEBUG_HTTP, c->log, 0,
                           "http chunked client request body recv %z", n);

            if (n == NGX_AGAIN) {
                break;
            }

            if (n == 0) {
                ngx_log_error(NGX_LOG_INFO, c->log, 0,
                              "client closed prematurely connection");
            }

            if (n == 0 || n == NGX_ERROR) {
                c->error = 1;
                return NGX_HTTP_BAD_REQUEST;
            }

            rc = ngx_http_chunkin_run_chunked_parser(r, ctx,
                    rb->buf->last, rb->buf->last + n);

            if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
                return rc;
            }

            if (rc == NGX_ERROR) {
                /* chunked body parsefail */
                return NGX_HTTP_BAD_REQUEST;
            }

            rb->buf->last += n;

            r->request_length += n;

            if (rc == NGX_OK) {
                done = 1;
                break;
            }

            if (rb->buf->last < rb->buf->end) {
                break;
            }
        }

        if (done) {
            break;
        }

        if (!c->read->ready) {
            clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);
            ngx_add_timer(c->read, clcf->client_body_timeout);

            if (ngx_handle_read_event(c->read, 0) != NGX_OK) {
                return NGX_HTTP_INTERNAL_SERVER_ERROR;
            }

            return NGX_AGAIN;
        }
    }

    if (c->read->timer_set) {
        ngx_del_timer(c->read);
    }

    if (rb->temp_file || r->request_body_in_file_only) {

        /* save the last part */
        if (ngx_http_write_request_body(r, ctx->chunks) != NGX_OK) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        b = ngx_calloc_buf(r->pool);
        if (b == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        b->in_file = 1;
        b->file_pos = 0;
        b->file_last = rb->temp_file->file.offset;
        b->file = &rb->temp_file->file;

        rb->bufs = ctx->chunks;
        if (rb->bufs == NULL) {
            rb->bufs = ngx_alloc_chain_link(r->pool);
            if (rb->bufs == NULL) {
                return NGX_HTTP_INTERNAL_SERVER_ERROR;
            }
        }

        rb->bufs->next = NULL;
        rb->bufs->buf = b;

    } else if ( r->request_body_in_single_buf ) {
        dd("request body in single buf");

        size = 0;
        for (cl = ctx->chunks; cl != NULL; cl = cl->next) {
            size += cl->buf->last - cl->buf->pos;
        }

        rb->buf = ngx_create_temp_buf(r->pool, size);

        if (rb->buf == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        for (cl = ctx->chunks; cl != NULL; cl = cl->next) {
            size = cl->buf->last - cl->buf->pos;
            memmove(rb->buf->last, cl->buf->pos, size);
            rb->buf->last += size;
        }

        rb->bufs = ctx->chunks;
        if (rb->bufs) {
            rb->bufs->next = NULL;
            rb->bufs->buf = rb->buf;
        }

    } else {
        rb->bufs = ctx->chunks;
    }

    dd("last minute, chunks_total_size: %d", ctx->chunks_total_size);

    rc = ngx_http_chunkin_set_content_length_header(r, ctx->chunks_total_size);

    if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        return rc;
    }

    rb->post_handler(r);

    return NGX_OK;
}

static ngx_int_t
ngx_http_test_expect(ngx_http_request_t *r)
{
    ngx_int_t   n;
    ngx_str_t  *expect;

    if (r->expect_tested
        || r->headers_in.expect == NULL
        || r->http_version < NGX_HTTP_VERSION_11)
    {
        return NGX_OK;
    }

    r->expect_tested = 1;

    expect = &r->headers_in.expect->value;

    if (expect->len != sizeof("100-continue") - 1
        || ngx_strncasecmp(expect->data, (u_char *) "100-continue",
                           sizeof("100-continue") - 1)
           != 0)
    {
        return NGX_OK;
    }

    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "send 100 Continue");

    n = r->connection->send(r->connection,
                            (u_char *) "HTTP/1.1 100 Continue" CRLF CRLF,
                            sizeof("HTTP/1.1 100 Continue" CRLF CRLF) - 1);

    if (n == sizeof("HTTP/1.1 100 Continue" CRLF CRLF) - 1) {
        return NGX_OK;
    }

    /* we assume that such small packet should be send successfully */

    return NGX_ERROR;
}

static ngx_int_t
ngx_http_write_request_body(ngx_http_request_t *r, ngx_chain_t *body)
{
    ssize_t                    n;
    ngx_temp_file_t           *tf;
    ngx_http_request_body_t   *rb;
    ngx_http_core_loc_conf_t  *clcf;

    rb = r->request_body;

    if (rb->temp_file == NULL) {
        tf = ngx_pcalloc(r->pool, sizeof(ngx_temp_file_t));
        if (tf == NULL) {
            return NGX_ERROR;
        }

        clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);

        tf->file.fd = NGX_INVALID_FILE;
        tf->file.log = r->connection->log;
        tf->path = clcf->client_body_temp_path;
        tf->pool = r->pool;
        tf->warn = "a client request body is buffered to a temporary file";
        tf->log_level = r->request_body_file_log_level;
        tf->persistent = r->request_body_in_persistent_file;
        tf->clean = r->request_body_in_clean_file;

        if (r->request_body_file_group_access) {
            tf->access = 0660;
        }

        rb->temp_file = tf;
    }

    if (body == NULL) {
        return NGX_OK;
    }

    n = ngx_write_chain_to_temp_file(rb->temp_file, body);

    /* TODO: n == 0 or not complete and level event */

    if (n == NGX_ERROR) {
        return NGX_ERROR;
    }

    rb->temp_file->offset += n;

    return NGX_OK;
}

