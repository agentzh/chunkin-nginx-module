#ifndef NGX_HTTP_CHUNKIN_UTIL_H
#define NGX_HTTP_CHUNKIN_UTIL_H

#include <ngx_http.h>

void ngx_http_chunkin_clear_transfer_encoding(ngx_http_request_t *r);

ngx_int_t ngx_http_chunkin_set_content_length_header(ngx_http_request_t *r, size_t len);

#endif /* NGX_HTTP_CHUNKIN_UTIL_H */

