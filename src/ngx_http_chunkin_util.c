#define DDEBUG 1

#include "ddebug.h"

#include "ngx_http_chunkin_util.h"

static ngx_str_t ngx_http_chunkin_content_length_header_key =
    ngx_string("Content-Length");

void
ngx_http_chunkin_clear_transfer_encoding(ngx_http_request_t *r)
{
    if (r->headers_in.transfer_encoding) {
        r->headers_in.transfer_encoding->hash = 0;
        r->headers_in.transfer_encoding = NULL;
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

