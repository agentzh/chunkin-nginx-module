#define DDEBUG 0

#include "ddebug.h"
#include "ngx_http_chunkin_request_body.h"

ngx_int_t
ngx_http_chunkin_read_chunked_request_body(ngx_http_request_t *r,
    ngx_http_client_body_handler_pt post_handler)
{
    /* TODO */
    return ngx_http_read_client_request_body(r, post_handler);
}

