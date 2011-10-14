/* Copyright (C) agentzh */

#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"

#include "ngx_http_chunkin_util.h"


static ngx_int_t ngx_http_chunkin_rm_header(ngx_list_t *l, ngx_table_elt_t *h);
static ngx_int_t ngx_http_chunkin_rm_header_helper(ngx_list_t *l,
    ngx_list_part_t *cur, ngx_uint_t i);


static ngx_str_t ngx_http_chunkin_content_length_header_key =
    ngx_string("Content-Length");


void
ngx_http_chunkin_clear_transfer_encoding(ngx_http_request_t *r)
{
    ngx_int_t           rc;

    if (r->headers_in.transfer_encoding) {
        rc = ngx_http_chunkin_rm_header(&r->headers_in.headers,
                                        r->headers_in.transfer_encoding);

        if (rc == NGX_OK) {
            r->headers_in.transfer_encoding = NULL;
        } else {
            /* not found */

            r->headers_in.transfer_encoding->value.len = 0;
            r->headers_in.transfer_encoding->value.data = (u_char *) "";
            r->headers_in.transfer_encoding->hash = 0;
            r->headers_in.transfer_encoding = NULL;
        }
    }
}


ngx_int_t
ngx_http_chunkin_set_content_length_header(ngx_http_request_t *r, size_t len) {
    ngx_table_elt_t             *h;
    ngx_list_part_t             *part;
    ngx_uint_t                  i;

    r->headers_in.content_length_n = len;

    part = &r->headers_in.headers.part;
    h = part->elts;

    for (i = 0; /* void */; i++) {

        if (i >= part->nelts) {
            if (part->next == NULL) {
                break;
            }

            part = part->next;
            h = part->elts;
            i = 0;
        }

        /*
        ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                "header value [%V]", &h[i].value);
        dd("header value (raw): [%s]", h[i].value.data);
        */

        if (h[i].key.len == ngx_http_chunkin_content_length_header_key.len
                && ngx_strncasecmp(h[i].key.data,
                    ngx_http_chunkin_content_length_header_key.data,
                    h[i].key.len) == 0)
        {
            dd("Found existing content-length header.");

            h[i].value.data = ngx_palloc(r->pool, NGX_OFF_T_LEN);

            if (h[i].value.data == NULL) {
                return NGX_HTTP_INTERNAL_SERVER_ERROR;
            }

            h[i].value.len = ngx_sprintf(h[i].value.data, "%O",
                    r->headers_in.content_length_n) - h[i].value.data;

            return NGX_OK;
        }
    }

    h = ngx_list_push(&r->headers_in.headers);

    if (h == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    h->hash = r->header_hash;

    h->key = ngx_http_chunkin_content_length_header_key;

    h->value.data = ngx_palloc(r->pool, NGX_OFF_T_LEN);

    if (h->value.data == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    h->value.len = ngx_sprintf(h->value.data, "%O",
            r->headers_in.content_length_n) - h->value.data;

    h->lowcase_key = ngx_pnalloc(r->pool, h->key.len);
    if (h->lowcase_key == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    ngx_strlow(h->lowcase_key, h->key.data, h->key.len);

    r->headers_in.content_length = h;

    return NGX_OK;
}


ngx_chain_t *
ngx_http_chunkin_get_buf(ngx_pool_t *pool, ngx_http_chunkin_ctx_t *ctx)
{
    ngx_chain_t     *cl;
    ngx_chain_t     **ll;
    ngx_uint_t      i;

    cl = ctx->free_bufs;

    if (cl == NULL) {
        ll = &ctx->free_bufs;
        for (i = 0; i < 4; i++) {
            cl = ngx_alloc_chain_link(pool);
            if (cl == NULL) {
                return NULL;
            }

            cl->buf = ngx_calloc_buf(pool);
            if (cl->buf == NULL) {
                return NULL;
            }

            cl->next = NULL;
            *ll = cl;
            ll = &cl->next;
        }
        cl = ctx->free_bufs;
    }

    if (cl) {
        ctx->free_bufs = cl->next;

        cl->buf->shadow = NULL;
        cl->next = NULL;

        dd("returned free buf");
        return cl;
    }

    dd("allocate new buf");

    cl = ngx_alloc_chain_link(pool);
    if (cl == NULL) {
        return NULL;
    }

    cl->buf = ngx_calloc_buf(pool);
    if (cl->buf == NULL) {
        return NULL;
    }

    cl->next = NULL;

    /* cl->buf->tag = (ngx_buf_tag_t) &ngx_http_chunkin_filter_module; */

    return cl;
}


/* modified from the ngx_http_internal_redirect function
 * in ngx_http_core_module.c of nginx 0.8.29.
 * copyrighted by Igor Sysoev. */
ngx_int_t
ngx_http_chunkin_restart_request(ngx_http_request_t *r,
        ngx_http_chunkin_ctx_t *ctx)
{
    ngx_log_debug2(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "chunkin: restart request: \"%V?%V\"",
                   &r->uri, &r->args);

    ngx_memzero(r->ctx, sizeof(void *) * ngx_http_max_module);

    ngx_http_set_ctx(r, ctx, ngx_http_chunkin_filter_module);

    r->internal = 0;
    /* r->phase_handler = 0; */

    ngx_http_handler(r);

#if defined(nginx_version) && nginx_version >= 8011

    dd("DISCARD BODY: %d", (int)r->discard_body);

    if ( ! ctx->r_discard_body ) {
        r->main->count++;
    }

#endif

    return NGX_DONE;
}


/* mostly a clone of the ngx_http_process_request_header function
 * in ngx_http_request.c of nginx 0.8.29.
 * copyrighted by Igor Sysoev. */
ngx_int_t
ngx_http_chunkin_process_request_header(ngx_http_request_t *r)
{
    dd("entered process_request_header");

    if (r->headers_in.host == NULL && r->http_version > NGX_HTTP_VERSION_10) {
        ngx_log_error(NGX_LOG_INFO, r->connection->log, 0,
                   "client sent HTTP/1.1 request without \"Host\" header");
        return NGX_HTTP_BAD_REQUEST;
    }

    if (r->headers_in.content_length) {
        r->headers_in.content_length_n =
                            ngx_atoof(r->headers_in.content_length->value.data,
                                      r->headers_in.content_length->value.len);

        if (r->headers_in.content_length_n == NGX_ERROR) {
            ngx_log_error(NGX_LOG_INFO, r->connection->log, 0,
                          "client sent invalid \"Content-Length\" header");
            return NGX_HTTP_LENGTH_REQUIRED;
        }
    }

    dd("method: %d (%d)", (int)r->method, (int)NGX_HTTP_PUT);
    dd("content_length_n: %d (%d)", (int)r->headers_in.content_length_n, -1);

    if (r->method & NGX_HTTP_PUT && r->headers_in.content_length_n == -1) {
        ngx_log_error(NGX_LOG_INFO, r->connection->log, 0,
                  "client sent %V method without \"Content-Length\" header",
                  &r->method_name);
        return NGX_HTTP_LENGTH_REQUIRED;
    }

    if (r->method & NGX_HTTP_TRACE) {
        ngx_log_error(NGX_LOG_INFO, r->connection->log, 0,
                      "client sent TRACE method");
        return NGX_HTTP_NOT_ALLOWED;
    }

    if (r->headers_in.transfer_encoding
        && ngx_strcasestrn(r->headers_in.transfer_encoding->value.data,
                           "chunked", 7 - 1))
    {
        ngx_log_error(NGX_LOG_INFO, r->connection->log, 0,
                      "client sent \"Transfer-Encoding: chunked\" header");
        return NGX_HTTP_LENGTH_REQUIRED;
    }

    if (r->headers_in.connection_type == NGX_HTTP_CONNECTION_KEEP_ALIVE) {
        if (r->headers_in.keep_alive) {
            r->headers_in.keep_alive_n =
                            ngx_atotm(r->headers_in.keep_alive->value.data,
                                      r->headers_in.keep_alive->value.len);
        }
    }

    return NGX_OK;
}


/* mostly a clone of the ngx_http_process_request function
 * in ngx_http_request.c of nginx 0.8.29.
 * copyrighted by Igor Sysoev. */
ngx_int_t
ngx_http_chunkin_process_request(ngx_http_request_t *r)
{
    ngx_connection_t  *c;

    c = r->connection;

    if (r->plain_http) {
        ngx_log_error(NGX_LOG_INFO, c->log, 0,
                      "client sent plain HTTP request to HTTPS port");
        /* ngx_http_finalize_request(r, NGX_HTTP_TO_HTTPS); */
        return NGX_HTTP_TO_HTTPS;
    }

#if (NGX_HTTP_SSL)

    if (c->ssl) {
        long                      rc;
        X509                     *cert;
        ngx_http_ssl_srv_conf_t  *sscf;

        sscf = ngx_http_get_module_srv_conf(r, ngx_http_ssl_module);

        if (sscf->verify) {
            rc = SSL_get_verify_result(c->ssl->connection);

            if (rc != X509_V_OK) {
                ngx_log_error(NGX_LOG_INFO, c->log, 0,
                              "client SSL certificate verify error: (%l:%s)",
                              rc, X509_verify_cert_error_string(rc));

                ngx_ssl_remove_cached_session(sscf->ssl.ctx,
                                       (SSL_get0_session(c->ssl->connection)));

                /* ngx_http_finalize_request(r, NGX_HTTPS_CERT_ERROR); */
                return NGX_HTTPS_CERT_ERROR;
            }

            if (sscf->verify == 1) {
                cert = SSL_get_peer_certificate(c->ssl->connection);

                if (cert == NULL) {
                    ngx_log_error(NGX_LOG_INFO, c->log, 0,
                                  "client sent no required SSL certificate");

                    ngx_ssl_remove_cached_session(sscf->ssl.ctx,
                                       (SSL_get0_session(c->ssl->connection)));

                    /* ngx_http_finalize_request(r, NGX_HTTPS_NO_CERT); */
                    return NGX_HTTPS_NO_CERT;
                }

                X509_free(cert);
            }
        }
    }

#endif

    if (c->read->timer_set) {
        ngx_del_timer(c->read);
    }

#if (NGX_STAT_STUB)
    (void) ngx_atomic_fetch_add(ngx_stat_reading, -1);
    r->stat_reading = 0;
    (void) ngx_atomic_fetch_add(ngx_stat_writing, 1);
    r->stat_writing = 1;
#endif

    return NGX_OK;
}


ngx_int_t
ngx_http_chunkin_internal_redirect(ngx_http_request_t *r,
    ngx_str_t *uri, ngx_str_t *args, ngx_http_chunkin_ctx_t *ctx)
{
    ngx_http_core_srv_conf_t  *cscf;

    r->uri_changes--;

    if (r->uri_changes == 0) {
        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                      "rewrite or internal redirection cycle "
                      "while internal redirect to \"%V\"", uri);

#if defined(nginx_version) && nginx_version >= 8011

        r->main->count++;

#endif

        ngx_http_finalize_request(r, NGX_HTTP_INTERNAL_SERVER_ERROR);
        return NGX_DONE;
    }

    r->uri = *uri;

    if (args) {
        r->args = *args;

    } else {
        r->args.len = 0;
        r->args.data = NULL;
    }

    ngx_log_debug2(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
                   "internal redirect: \"%V?%V\"", uri, &r->args);

    ngx_http_set_exten(r);

    /* clear the modules contexts */
    ngx_memzero(r->ctx, sizeof(void *) * ngx_http_max_module);

    ngx_http_set_ctx(r, ctx, ngx_http_chunkin_filter_module);

    cscf = ngx_http_get_module_srv_conf(r, ngx_http_core_module);
    r->loc_conf = cscf->ctx->loc_conf;

    ngx_http_update_location_config(r);

#if (NGX_HTTP_CACHE)
    r->cache = NULL;
#endif

    r->internal = 0;

#if defined(nginx_version) && nginx_version >= 8011

    dd("DISCARD BODY: %d", (int)r->discard_body);

    if ( ! ctx->r_discard_body ) {
        r->main->count++;
    }

#endif

    ngx_http_handler(r);

    return NGX_DONE;
}


static ngx_int_t
ngx_http_chunkin_rm_header(ngx_list_t *l, ngx_table_elt_t *h)
{
    ngx_uint_t                   i;
    ngx_list_part_t             *part;
    ngx_table_elt_t             *data;

    part = &l->part;
    data = part->elts;

    for (i = 0; /* void */; i++) {
        dd("i: %d, part: %p", (int) i, part);

        if (i >= part->nelts) {
            if (part->next == NULL) {
                break;
            }

            part = part->next;
            h = part->elts;
            i = 0;
        }

        if (&data[i] == h) {
            dd("found header");

            return ngx_http_chunkin_rm_header_helper(l, part, i);
        }
    }

    return NGX_ERROR;
}


static ngx_int_t
ngx_http_chunkin_rm_header_helper(ngx_list_t *l, ngx_list_part_t *cur,
        ngx_uint_t i)
{
    ngx_table_elt_t             *data;
    ngx_list_part_t             *new, *part;

    dd("list rm item: part %p, i %d, nalloc %d", cur, (int) i,
            (int) l->nalloc);

    data = cur->elts;

    dd("cur: nelts %d, nalloc %d", (int) cur->nelts,
            (int) l->nalloc);

    if (i == 0) {
        cur->elts = (char *) cur->elts + l->size;
        cur->nelts--;

        if (cur == l->last) {
            if (l->nalloc > 1) {
                l->nalloc--;
                return NGX_OK;
            }

            /* l->nalloc == 1 */

            part = &l->part;
            while (part->next != cur) {
                if (part->next == NULL) {
                    return NGX_ERROR;
                }
                part = part->next;
            }

            part->next = NULL;
            l->last = part;

            return NGX_OK;
        }

        if (cur->nelts == 0) {
            part = &l->part;
            while (part->next != cur) {
                if (part->next == NULL) {
                    return NGX_ERROR;
                }
                part = part->next;
            }

            part->next = cur->next;

            return NGX_OK;
        }

        return NGX_OK;
    }

    if (i == cur->nelts - 1) {
        cur->nelts--;

        if (cur == l->last) {
            l->nalloc--;
        }

        return NGX_OK;
    }

    new = ngx_palloc(l->pool, sizeof(ngx_list_part_t));
    if (new == NULL) {
        return NGX_ERROR;
    }

    new->elts = &data[i + 1];
    new->nelts = cur->nelts - i - 1;
    new->next = cur->next;

    l->nalloc = new->nelts;

    cur->nelts = i;
    cur->next = new;
    if (cur == l->last) {
        l->last = new;
    }

    cur = new;

    return NGX_OK;
}

