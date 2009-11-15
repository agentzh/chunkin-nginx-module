#define DDEBUG 1

#include "ddebug.h"

#include "chunked_parser.h"

%% machine chunked;
%% write data;

ngx_int_t
ngx_http_chunkin_init_chunked_parser(ngx_http_request_t *r,
        ngx_http_chunkin_ctx_t *ctx)
{
    int cs;

    %% write init;

    ctx->chunks = NULL;
    ctx->next_chunk = NULL;
    ctx->chunk = NULL;
    ctx->chunk_size = 0;
    ctx->chunk_size_order = 0;
    ctx->chunk_bytes_read = 0;

    ctx->chunks_total_size = 0;

    ctx->parser_state = cs;

    return NGX_OK;
}

ngx_int_t
ngx_http_chunkin_run_chunked_parser(ngx_http_request_t *r,
        ngx_http_chunkin_ctx_t *ctx, u_char *pos, u_char *last)
{
    int                 cs   = ctx->parser_state;
    ngx_connection_t    *c   = r->connection;
    u_char              *p   = pos;
    u_char              *pe  = last;
    u_char              *eof = NULL;
    ngx_buf_t           *b;

    %%{
        action bad_chunked {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                    "bad chunked body (offset %O).\n", p - pos);
        }

        action finish {
            ctx->parser_state = chunked_first_final;
            return NGX_OK;
        }

        action bad_chunk_data {
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                "bad chunk data "
                "(bytes already read %uz, bytes expected: %uz): "
                "offset %O.\n", ctx->chunk_bytes_read,
                ctx->chunk_size, p - pos);
        }

        action test_len {
            ctx->chunk_bytes_read < ctx->chunk_size
        }

        action read_data_byte {
            ctx->chunk_bytes_read++;

            dd("has rb? %d", r->request_body ? 1 : 0);
            dd("has rb->buf? %d", r->request_body->buf ? 1 : 0);
            if (r->request_body && r->request_body->buf && ctx->chunk->buf->last >= r->request_body->buf->last) {
                dd("buf overflows!");
            }
            ctx->chunk->buf->last++;
            ctx->chunk->buf->end++;

            ctx->chunks_total_size++;

            dd("bytes read: %d", ctx->chunk->buf->last - ctx->chunk->buf->pos);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }

        action start_reading_size {
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;

            ctx->chunk = NULL;
        }

        action read_size {
            ctx->chunk_size <<= 4;
            ctx->chunk_size_order++;
            if (*p >= 'A' && *p <= 'F') {
                ctx->chunk_size |= 10 + *p - 'A';
            } else if (*p >= 'a' && *p <= 'f') {
                ctx->chunk_size |= 10 + *p - 'a';
            } else {
                ctx->chunk_size |= *p - '0';
            }

            ngx_log_debug1(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: chunk size: %uz\n", ctx->chunk_size);
        }

        action start_reading_data {
            if (ctx->next_chunk == NULL || *ctx->next_chunk == NULL) {
                b = ngx_calloc_buf(r->pool);

                if (b == NULL) {
                    ctx->parser_state = chunked_error;
                    return NGX_HTTP_INTERNAL_SERVER_ERROR;
                }

                ctx->chunk = ngx_alloc_chain_link(r->pool);
                if (ctx->chunk == NULL) {
                    ctx->parser_state = chunked_error;
                    return NGX_HTTP_INTERNAL_SERVER_ERROR;
                }
                ctx->chunk->buf = b;
                ctx->chunk->next = NULL;

                if (ctx->chunks == NULL) {
                    ctx->chunks = ctx->chunk;
                } else {
                    *ctx->next_chunk = ctx->chunk;
                }

            } else {
                ctx->chunk = *ctx->next_chunk;
                b = ctx->chunk->buf;
            }

            ctx->chunks_count++;

            ctx->next_chunk = &ctx->chunk->next;

            b->end = b->last = b->pos = b->start = p;
            b->memory = 1;
            b->sync = 0;
        }

        action verify_data {
            if (ctx->chunk_bytes_read != ctx->chunk_size) {
                ngx_log_error(NGX_LOG_ERR, c->log, 0,
                        "ERROR: chunk size not meet: "
                        "%uz != %uz\n", ctx->chunk_bytes_read,
                        ctx->chunk_size);
                fbreak;
            }
            ctx->chunk_size = -1;
        }

        CRLF = "\r\n";

        chunk_size = (xdigit+ - "0") >start_reading_size $read_size;

        chunk_data_octet = any
                $read_data_byte
                when test_len $err(bad_chunk_data);

        chunk_data = chunk_data_octet*
                >start_reading_data
                %verify_data;

        chunk = chunk_size " "* CRLF
                        chunk_data CRLF;

        last_chunk = "0" " "* CRLF;

        main := chunk*
                last_chunk
                CRLF @err(bad_chunked) %err(finish);

    }%%

    %% write exec;

    ctx->parser_state = cs;

    if (cs >= chunked_first_final) {
        return NGX_OK;
    }

    if (cs == chunked_error) {
        return NGX_ERROR;
    }

    return NGX_AGAIN;
}

