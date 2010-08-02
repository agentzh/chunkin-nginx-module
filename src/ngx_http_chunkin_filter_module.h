#ifndef NGX_HTTP_CHUNKIN_FILTER_MODULE_H
#define NGX_HTTP_CHUNKIN_FILTER_MODULE_H

#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>

typedef struct {
    ngx_flag_t          enabled;
    ngx_flag_t          keepalive;
    ngx_uint_t          max_chunks_per_buf;

} ngx_http_chunkin_conf_t;


typedef struct {
    ngx_flag_t          ignore_body /* for output filters only */;
    int                 parser_state;  /* current state */

    size_t              chunk_bytes_read;
    size_t              chunk_size;
    int                 chunk_size_order;

    size_t              chunks_total_size;
    size_t              chunks_written_size;
    ngx_uint_t          chunks_count;

    off_t               raw_body_size;

    ngx_chain_t         *chunks;
    ngx_chain_t         **next_chunk;
    ngx_chain_t         *chunk;

    ngx_flag_t          just_after_preread;

    ngx_chain_t         *free_bufs;
    ngx_chain_t         *last_complete_chunk;

    u_char              *saved_header_in_pos;

    ngx_uint_t          count;
    ngx_flag_t          r_discard_body:1;

    ngx_flag_t          done:1;
    ngx_flag_t          waiting_more_body:1;

} ngx_http_chunkin_ctx_t;


extern ngx_module_t  ngx_http_chunkin_filter_module;

#endif /* NGX_HTTP_CHUNKIN_FILTER_MODULE_H */

