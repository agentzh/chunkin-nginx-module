#ifndef CHUNKIN_CHUNKED_PARSER_H
#define CHUNKIN_CHUNKED_PARSER_H

#include "ngx_http_chunkin_filter_module.h"

ngx_int_t ngx_http_chunkin_init_chunked_parser(ngx_http_request_t *r,
        ngx_http_chunkin_ctx_t *ctx);

ngx_int_t ngx_http_chunkin_run_chunked_parser(ngx_http_request_t *r,
        ngx_http_chunkin_ctx_t *ctx, u_char *pos, u_char *last);

#endif /* CHUNKIN_CHUNKED_PARSER_H */

