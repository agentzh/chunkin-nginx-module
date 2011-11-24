#ifndef NGX_HTTP_CHUNKIN_UTIL_H
#define NGX_HTTP_CHUNKIN_UTIL_H

#include <ngx_http.h>
#include "ngx_http_chunkin_filter_module.h"

void ngx_http_chunkin_clear_transfer_encoding(ngx_http_request_t *r);

ngx_int_t ngx_http_chunkin_set_content_length_header(ngx_http_request_t *r,
        size_t len);

ngx_chain_t * ngx_http_chunkin_get_buf(ngx_pool_t *pool,
        ngx_http_chunkin_ctx_t *ctx);

ngx_int_t ngx_http_chunkin_restart_request(ngx_http_request_t *r,
        ngx_http_chunkin_ctx_t *ctx);

ngx_int_t ngx_http_chunkin_process_request_header(ngx_http_request_t *r);

ngx_int_t ngx_http_chunkin_process_request(ngx_http_request_t *r);

ngx_int_t ngx_http_chunkin_internal_redirect(ngx_http_request_t *r,
    ngx_str_t *uri, ngx_str_t *args, ngx_http_chunkin_ctx_t *ctx);

#endif /* NGX_HTTP_CHUNKIN_UTIL_H */

