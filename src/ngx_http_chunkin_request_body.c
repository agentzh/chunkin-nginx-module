#define DDEBUG 1

#include "ddebug.h"

#include "chunked_parser.h"

#include "ngx_http_chunkin_filter_module.h"
#include "ngx_http_chunkin_request_body.h"

static ngx_int_t ngx_http_test_expect(ngx_http_request_t *r);

static void ngx_http_chunkin_read_chunked_request_body_handler(
        ngx_http_request_t *r);

static ngx_int_t ngx_http_chunkin_do_read_chunked_request_body(
        ngx_http_request_t *r);

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
    ngx_chain_t                 *cl, **next;
    ngx_http_core_loc_conf_t    *clcf;
    ngx_buf_t                   *b;
    ngx_http_chunkin_ctx_t      *ctx;
    ngx_int_t                   rc;

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
            return NGX_HTTP_BAD_REQUEST;
        }

        if (rc == NGX_OK) {
            dd("we have the whole chunked body in 'prepread'...");

            if (ctx->chunks == NULL) {
                dd("zero-size chunk found.");
                return NGX_OK;
            }

            rb->bufs = ctx->chunks;
            rb->buf = ctx->chunks->buf;

            dd("buf len: %d", rb->buf->last - rb->buf->pos);

            r->header_in->pos = r->header_in->last;

            r->request_length += preread;

            return NGX_OK;
        }

        if (rc == NGX_AGAIN) {
            dd("we need to read more chunked body addition to 'prepread'...");

            rb->bufs = ctx->chunks;
            rb->buf = ctx->chunks->buf;

            dd("buf len: %d", rb->buf->last - rb->buf->pos);
            return NGX_OK;
        }

        b = ngx_calloc_buf(r->pool);
        if (b == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        b->temporary = 1;
        b->start = r->header_in->pos;
        b->pos = r->header_in->pos;
        b->last = r->header_in->last;
        b->end = r->header_in->end;

        rb->bufs = ngx_alloc_chain_link(r->pool);
        if (rb->bufs == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        rb->bufs->buf = b;
        rb->bufs->next = NULL;

        r->header_in->pos = r->header_in->last;

        r->request_length += preread;

        next = &rb->bufs->next;
    } else {
        b = NULL;
        next = &rb->bufs;
    }

    size = clcf->client_body_buffer_size;

    rb->buf = ngx_create_temp_buf(r->pool, size);

    if (rb->buf == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    cl = ngx_alloc_chain_link(r->pool);
    if (cl == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    cl->buf = rb->buf;
    cl->next = NULL;

    if (b && r->request_body_in_single_buf) {
        size = b->last - b->pos;
        ngx_memcpy(rb->buf->pos, b->pos, size);
        rb->buf->last += size;

        next = &rb->bufs; /* "next" used to be "&rb->bufs->next" */
    }

    *next = cl;

    if (r->request_body_in_file_only || r->request_body_in_single_buf) {
        rb->to_write = rb->bufs;

    } else {
        rb->to_write = rb->bufs->next ? rb->bufs->next : rb->bufs;
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
#if 0

    size_t                     size;
    ssize_t                    n;
    ngx_buf_t                 *b;
    ngx_connection_t          *c;
    ngx_http_request_body_t   *rb;
    ngx_http_core_loc_conf_t  *clcf;

    c = r->connection;
    rb = r->request_body;

    ngx_log_debug0(NGX_LOG_DEBUG_HTTP, c->log, 0,
                   "http chunkin read chunked client request body");
    for ( ;; ) {
        for ( ;; ) {
            if (rb->buf->last == rb->buf->end) {
            }
        }
    }

#endif
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

