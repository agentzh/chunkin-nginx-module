/* Copyright (C) agentzh */

#define DDEBUG 0

#include "ddebug.h"

#include "ngx_http_chunkin_util.h"
#include "chunked_parser.h"
#include "ngx_http_chunkin_filter_module.h"
#include "ngx_http_chunkin_request_body.h"

enum {
    MAX_CHUNKS_PER_DISK_WRITE = 512
};

static ngx_int_t ngx_http_test_expect(ngx_http_request_t *r);

static void ngx_http_chunkin_read_chunked_request_body_handler(
        ngx_http_request_t *r);

static ngx_int_t ngx_http_chunkin_do_read_chunked_request_body(
        ngx_http_request_t *r);

static ngx_int_t ngx_http_write_request_body(ngx_http_request_t *r,
        ngx_chain_t *body, int chain_count);


/* this function's implementation is borrowed from nginx 0.8.20
 * and modified a lot to work with the chunked encoding.
 * copyrighted (C) by Igor Sysoev */
ngx_int_t
ngx_http_chunkin_read_chunked_request_body(ngx_http_request_t *r,
    ngx_http_client_body_handler_pt post_handler)
{
    size_t                      preread;
    size_t                      size;
    ngx_http_request_body_t     *rb;
    ngx_http_core_loc_conf_t    *clcf;
    ngx_http_chunkin_ctx_t      *ctx;
    ngx_int_t                   rc;

    /* r->request_body_in_single_buf = 1; */
    /* r->request_body_in_file_only = 1; */

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
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    ctx->count++;

    ctx->saved_header_in_pos = r->header_in->pos;

    preread = r->header_in->last - r->header_in->pos;

    ngx_http_chunkin_init_chunked_parser(r, ctx);

    if (preread) {
        ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                       "chunkin: http chunked client request body preread %uz",
                       preread);

        dd("raw chunked body %.*s", preread, r->header_in->pos);

        rc = ngx_http_chunkin_run_chunked_parser(r, ctx,
                &r->header_in->pos, r->header_in->last, "preread");

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

            dd("keepalive? %s", r->keepalive ? "yes" : "no");

            dd("chunks total size: %d", ctx->chunks_total_size);

            rc = ngx_http_chunkin_set_content_length_header(r,
                    ctx->chunks_total_size);

            if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
                return rc;
            }

            rb->bufs = ctx->chunks;
            if (rb->bufs) {
                rb->buf = ctx->chunks->buf;
            }

            dd("buf len: %d", rb->buf->last - rb->buf->pos);

            dd("buf left (len %d): %s",
                    (int) (r->header_in->last - r->header_in->pos),
                    r->header_in->pos);

            /* r->header_in->pos = r->header_in->last; */

            r->request_length += r->header_in->pos
                - ctx->saved_header_in_pos;

            ctx->raw_body_size += r->header_in->pos
                - ctx->saved_header_in_pos;

            post_handler(r);

            return NGX_OK;
        }

        if (rc != NGX_AGAIN) {
            /* this is impossible to reach... */
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        size = r->header_in->last - r->header_in->pos;
        if (size) {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                  "chunkin: internal assertion failed: %O bytes "
                  "left in the r->header_in buffer but "
                  "the parser returns NGX_AGAIN",
                  (off_t) size);

            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        dd("we need to read more chunked body in addition to 'prepread'...");

        ctx->just_after_preread = 1;

        /* r->header_in->pos = r->header_in->last; */

        r->request_length += preread;

        ctx->raw_body_size += preread;
    }

    size = clcf->client_body_buffer_size;
    if (ctx->chunks_total_size > size) {
        size = ctx->chunks_total_size;
    }

    dd("chunks total size after preread: %d, buf size: %d",
            (int)ctx->chunks_total_size, (int)size);

    rb->buf = ngx_create_temp_buf(r->pool, size);

#if 0
    /* XXX just for debugging... */
    ngx_memzero(rb->buf->start, rb->buf->end - rb->buf->start);
#endif

    if (rb->buf == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    r->read_event_handler = ngx_http_chunkin_read_chunked_request_body_handler;

    rc = ngx_http_chunkin_do_read_chunked_request_body(r);

    return rc;
}


/* mostly a clone of the ngx_http_read_client_request_body_handler
 * function in ngx_http_request_body.c of nginx 0.8.20.
 * copyrighted by Igor Sysoev. */
static void
ngx_http_chunkin_read_chunked_request_body_handler(ngx_http_request_t *r)
{
    ngx_int_t  rc;

    if (r->connection->read->timedout) {
        r->connection->timedout = 1;

        (void) ngx_http_discard_request_body(r);

        ngx_http_finalize_request(r, NGX_HTTP_REQUEST_TIME_OUT);
        return;
    }

    rc = ngx_http_chunkin_do_read_chunked_request_body(r);

    if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        ngx_http_finalize_request(r, rc);
    }
}


/* mostly based on the ngx_http_do_read_client_request_body
 * function in ngx_http_request_body.c of nginx 0.8.20.
 * copyrighted by Igor Sysoev. */
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
    ngx_chain_t               *cl, *pending_chunk;
    u_char                    *p;
    ngx_http_chunkin_conf_t  *conf;

    c = r->connection;
    rb = r->request_body;
    ctx = ngx_http_get_module_ctx(r, ngx_http_chunkin_filter_module);

    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, c->log, 0,
                   "chunkin: http chunkin read chunked client request body");

    done = 0;

    clcf = ngx_http_get_module_loc_conf(r, ngx_http_core_module);

    if (ctx->just_after_preread) {
        ctx->just_after_preread = 0;

        dd("Just after preread and ctx->chunks defined (bytes read: %d, "
                "chunk size: %d, last chars %c %c %c)", ctx->chunk_bytes_read,
                ctx->chunk_size, *(r->header_in->pos - 2),
                *(r->header_in->pos - 1), *r->header_in->pos);

        for (cl = ctx->chunks; cl; cl = cl->next) {
            b = cl->buf;

            dd("before ngx_copy...");
            p = rb->buf->last;
            rb->buf->last = ngx_copy(rb->buf->last, b->pos, b->last - b->pos);
            dd("after ngx_copy...");

            b->pos = p;
            b->last = rb->buf->last;
        }
    }

    conf = ngx_http_get_module_loc_conf(r, ngx_http_chunkin_filter_module);

    for ( ;; ) {
        for ( ;; ) {
            /*
            dd("client_max_body_size: %d, raw_body_size: %d",
                    (int)clcf->client_max_body_size,
                    (int)ctx->raw_body_size);
                    */

            if (clcf->client_max_body_size
                    && clcf->client_max_body_size < ctx->raw_body_size)
            {
                ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                          "chunkin: client intended to send too large body: "
                          "%O bytes", (off_t) ctx->raw_body_size);

                return NGX_HTTP_REQUEST_ENTITY_TOO_LARGE;
            }

            /* dd("rb->buf pos: %d", (int) (rb->buf->last - rb->buf->pos)); */

            if (rb->buf->last == rb->buf->end
                    || ctx->chunks_count > conf->max_chunks_per_buf)
            {
                if (ctx->chunks_count > conf->max_chunks_per_buf) {
                    dd("too many chounks already: %d (max %d, buf last %c)",
                            (int) ctx->chunks_count,
                            (int) conf->max_chunks_per_buf,
                            *(rb->buf->last - 2));
                }

                if (ctx->chunks == NULL
                        || (ctx->chunks_total_size
                            && ctx->chunks_total_size <=
                            ctx->chunks_written_size))
                {
                    ngx_log_error(NGX_LOG_WARN, c->log, 0,
                          "chunkin: the chunkin_max_chunks_per_buf or "
                          "max_client_body_size setting seems rather small "
                          "(chunks %snull, total decoded %d, "
                          "total written %d)",
                          (u_char *) (ctx->chunks ? "not " : ""),
                          (int) ctx->chunks_total_size,
                          (int) ctx->chunks_written_size);

                } else {
                    dd("save exceeding part to disk (%d bytes), buf size: %d, "
                            "chunks count: %d",
                            ctx->chunks_total_size - ctx->chunks_written_size,
                            rb->buf->end - rb->buf->start,
                            ctx->chunks_count);

                    rc = ngx_http_write_request_body(r, ctx->chunks,
                            ctx->chunks_count);

                    if (rc != NGX_OK) {
                        return NGX_HTTP_INTERNAL_SERVER_ERROR;
                    }
                }

                if (ctx->last_complete_chunk) {
                    pending_chunk = ctx->last_complete_chunk->next;
                    /* add ctx->chunks ~ ctx->last_complete_chunk
                     * into ctx->free_bufs */
                    ctx->last_complete_chunk->next = ctx->free_bufs;
                    ctx->free_bufs = ctx->chunks;
                    ctx->last_complete_chunk = NULL;
                } else {
                    pending_chunk = ctx->chunks;
                }

                if (pending_chunk) {
                    ctx->next_chunk = &pending_chunk->next;
                    ctx->chunks = pending_chunk;
                    ctx->chunk = pending_chunk;

                    ctx->chunk->buf->pos = rb->buf->start;
                    ctx->chunk->buf->last = rb->buf->start;
                    ctx->chunks_count = 1;

                    ctx->chunks_written_size = ctx->chunks_total_size
                        - ngx_buf_size(pending_chunk->buf);
                } else {
                    ctx->next_chunk = NULL;

                    ctx->chunks = NULL;
                    ctx->chunk = NULL;

                    ctx->chunks_count = 0;

                    ctx->chunks_written_size = ctx->chunks_total_size;
                }

                dd("reset rb->buf");

                rb->buf->last = rb->buf->start;

#if 0
                /* XXX just for debugging... */
                ngx_memzero(rb->buf->start, rb->buf->end - rb->buf->start);
#endif
            }

            size = rb->buf->end - rb->buf->last;

            n = c->recv(c, rb->buf->last, size);
            ngx_log_debug1(NGX_LOG_DEBUG_HTTP, c->log, 0,
                           "chunkin: http chunked client request body recv %z",
                           n);

            if (n == NGX_AGAIN) {
                dd("NGX_AGAIN caught");
                break;
            }

            if (n == 0) {
                ngx_log_error(NGX_LOG_INFO, c->log, 0,
                              "chunkin: client closed prematurely connection");
            }

            if (n == 0 || n == NGX_ERROR) {
                c->error = 1;
                return NGX_HTTP_BAD_REQUEST;
            }

            /* save the original pos */
            p = rb->buf->last;

            rc = ngx_http_chunkin_run_chunked_parser(r, ctx,
                    &rb->buf->last, rb->buf->last + n, "main handler");

            if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
                return rc;
            }

            if (rc == NGX_ERROR) {
                /* chunked body parsefail */
                return NGX_HTTP_BAD_REQUEST;
            }

            /* rb->buf->last += n; */

            r->request_length += n;

            ctx->raw_body_size += n;

            if (rc == NGX_OK) {
                dd("successfully done the parsing");

                dd("keepalive? %s", r->keepalive ? "yes" : "no");

                if (r->keepalive) {
                    dd("cleaning the buffers for pipelined reqeusts (if any)");
                    size = p + n - rb->buf->last;
                    if (size) {
                        dd("found remaining data for pipelined requests");
                        if (size > (size_t) (r->header_in->end
                                    - ctx->saved_header_in_pos))
                        {
                            /* XXX enlarge the r->header_in buffer... */
                            r->keepalive = 0;
                        } else {
                            r->header_in->pos = ctx->saved_header_in_pos;
                            r->header_in->last = ngx_copy(r->header_in->pos,
                                    rb->buf->last, size);

                        }
                    }
                }

                done = 1;
                break;
            }

            /* rc == NGX_AGAIN */

            if (rb->buf->last < rb->buf->end) {
                break;
            }
        }

        if (done) {
            break;
        }

        if (!c->read->ready) {
            ngx_add_timer(c->read, clcf->client_body_timeout);

            if (ngx_handle_read_event(c->read, 0) != NGX_OK) {
                return NGX_HTTP_INTERNAL_SERVER_ERROR;
            }

            return NGX_AGAIN;
        }
    }

    if (!done) {
        return NGX_HTTP_BAD_REQUEST;
    }

    if (c->read->timer_set) {
        ngx_del_timer(c->read);
    }

    if (rb->temp_file || r->request_body_in_file_only) {
        size = ctx->chunks_total_size - ctx->chunks_written_size;
        dd("save the last part to disk...(%d bytes left)", size);
        if (size == 0) {
            ctx->chunks = NULL;
        }

#if 0
        n = 0;
        for (cl = ctx->chunks; cl != NULL; cl = cl->next) {
            /* dd("chunks %d found buf %c", n, *cl->buf->start); */
            n++;
        }
#endif

        dd("for total %d chunks found", n);

        /* save the last part */
        rc = ngx_http_write_request_body(r, ctx->chunks, ctx->chunks_count);
        if (rc != NGX_OK) {
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

        rb->bufs = ngx_http_chunkin_get_buf(r->pool, ctx);

        if (rb->bufs == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        rb->bufs->buf = b;

    } else if ( r->request_body_in_single_buf ) {
        dd("request body in single buf");

        /* XXX we may not have to allocate a big buffer here */

        size = 0;
        for (cl = ctx->chunks; cl != NULL; cl = cl->next) {
            size += ngx_buf_size(cl->buf);
        }

        rb->buf = ngx_create_temp_buf(r->pool, size);

        if (rb->buf == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        for (cl = ctx->chunks; cl != NULL; cl = cl->next) {
            size = ngx_buf_size(cl->buf);

            dd("copy buf ...(size %d)", size);

            rb->buf->last =
                ngx_cpymem(rb->buf->last, cl->buf->pos, size);
        }

        rb->bufs = ngx_http_chunkin_get_buf(r->pool, ctx);

        if (rb->bufs == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        rb->bufs->buf = rb->buf;

    } else {
        rb->bufs = ctx->chunks;
    }

#if 0
    if (rb->bufs) {
        int i;
        for (i = 0, cl = rb->bufs; cl; cl = cl->next, i++) {
            if (cl->buf->memory && cl->buf->pos == cl->buf->last) {
                dd("Found zero size buf in chain pos %d", i);
            }
        }
    }
#endif

    dd("last minute, chunks count: %d, chunks_total_size: %d",
            ctx->chunks_count, ctx->chunks_total_size);

#if 0
    size = 0;
    for (cl = rb->bufs; cl; cl = cl->next) {
        size += ngx_buf_size(cl->buf);
    }
    dd("data size: %d", size);
#endif

    rc = ngx_http_chunkin_set_content_length_header(r, ctx->chunks_total_size);

    if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        return rc;
    }

    rb->post_handler(r);

    return NGX_OK;
}


/* mostly an exact clone of the ngx_http_test_expect
 * function in ngx_http_request_body.c of nginx 0.8.20.
 * copyrighted by Igor Sysoev. */
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


/* mostly an exact clone of the ngx_http_write_request_body
 * function in ngx_http_request_body.c of nginx 0.8.20.
 * copyrighted by Igor Sysoev. */
static ngx_int_t
ngx_http_write_request_body(ngx_http_request_t *r, ngx_chain_t *body,
        int chain_count)
{
    ssize_t                    n;
    ngx_temp_file_t           *tf;
    ngx_http_request_body_t   *rb;
    ngx_http_core_loc_conf_t  *clcf;
    int                       i;
    ngx_chain_t               *cl, *saved_next;

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

        if (rb->buf && rb->buf->last == rb->buf->end) {
            tf->warn = "a client request body is buffered to a temporary file "
                "(exceeding client_body_buffer_size)";
        } else {
            tf->warn = "a client request body is buffered to a temporary file "
                "(exceeding chunkin_max_chunks_per_buf)";
        }

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

    /* we need this hack to work around a bug in
     * ngx_write_chain_to_temp_file when the chain
     * link is longer than 1024, we'll receive
     * random alert like this:
     *   [alert] 13493#0: *1 pread() read only 1024
     *   of 3603 from
     *   "/opt/nginx/client_body_temp/0000000001" */
    if (chain_count > MAX_CHUNKS_PER_DISK_WRITE) {
        i = 0;
        for (cl = body; cl; cl = cl->next) {
            if (i >= MAX_CHUNKS_PER_DISK_WRITE) {
                /* dd("wrote %d links first...", i+1); */

                saved_next = cl->next;
                cl->next = NULL;
                n = ngx_write_chain_to_temp_file(rb->temp_file, body);
                /* TODO: n == 0 or not complete and level event */

                if (n == NGX_ERROR) {
                    return NGX_ERROR;
                }

                rb->temp_file->offset += n;

                cl->next = saved_next;
                body = cl->next;
                i = 0;
            } else {
                i++;
            }
        }
    }

    if (body) {
        n = ngx_write_chain_to_temp_file(rb->temp_file, body);
        /* TODO: n == 0 or not complete and level event */

        if (n == NGX_ERROR) {
            return NGX_ERROR;
        }

        rb->temp_file->offset += n;
    }

    return NGX_OK;
}

