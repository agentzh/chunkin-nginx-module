#ifndef NGX_HTTP_CHUNKIN_REQUEST_BODY_H
#define NGX_HTTP_CHUNKIN_REQUEST_BODY_H

#include <ngx_http.h>

ngx_int_t ngx_http_chunkin_read_chunked_request_body(ngx_http_request_t *r,
    ngx_http_client_body_handler_pt post_handler);

#endif /* NGX_HTTP_CHUNKIN_REQUEST_BODY_H */

