#define DDEBUG 0

#include "ddebug.h"

#include "chunked_parser.h"
#include "ngx_http_chunkin_util.h"

enum {
    PRE_TEXT_LEN = 25,
    POST_TEXT_LEN = 25
};

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
        ngx_http_chunkin_ctx_t *ctx, u_char **pos_addr, u_char *last)
{
    int                 cs   = ctx->parser_state;
    ngx_connection_t    *c   = r->connection;
    u_char              *pos = *pos_addr;
    u_char              *p   = *pos_addr;
    u_char              *pe  = last;
    u_char              *eof = NULL;
    ngx_buf_t           *b;
    ngx_flag_t          done = 0;
    ngx_str_t           pre, post;
    char*               err_ctx = "";

    %%{
        alphtype short;

        action finalize {
            done = 1;
        }

        action test_len {
            ctx->chunk_bytes_read < ctx->chunk_size
        }

        action read_data_byte {
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }

        action start_reading_size {
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
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
            ctx->chunk = ngx_http_chunkin_get_buf(r->pool, ctx);

            if (ctx->chunks) {
                *ctx->next_chunk = ctx->chunk;
            } else {
                ctx->chunks = ctx->chunk;
            }

            ctx->next_chunk = &ctx->chunk->next;

            b = ctx->chunk->buf;

            b->last = b->pos = p;
            b->memory = 1;
        }

        action verify_data {
            if (ctx->chunk_bytes_read != ctx->chunk_size) {
                ngx_log_error(NGX_LOG_ERR, c->log, 0,
                        "ERROR: chunk size not met: "
                        "%uz != %uz\n", ctx->chunk_bytes_read,
                        ctx->chunk_size);
                *pos_addr = p;
                ctx->parser_state = chunked_error;
                return NGX_ERROR;
            }

            if (ctx->chunk_size == 0) {
                /* remove the current chunk */
                ctx->chunk->next = ctx->free_bufs;
                ctx->free_bufs = ctx->chunk;
                ctx->chunk = ctx->last_complete_chunk;
                if (ctx->last_complete_chunk) {
                    ctx->last_complete_chunk->next = NULL;
                } else {
                    ctx->chunks = NULL;
                }
            } else {
                ctx->last_complete_chunk = ctx->chunk;
            }
        }

        CRLF = "\r\n" $err{ err_ctx = "CRLF"; };

        chunk_size = (xdigit+ - "0"+) >start_reading_size $read_size;

        chunk_data_octet = any when test_len;

        chunk_data = (chunk_data_octet+)
                     >start_reading_data
                     $read_data_byte
                     $err{ err_ctx = "chunk_data"; }
                   ;

        chunk_data_terminator = "\r" when ! test_len "\n"
                                $err{ err_ctx = "chunk_data_terminator"; }
                              ;

        chunk = chunk_size " "* CRLF $err{ err_ctx = "chunk_size"; }
                        chunk_data chunk_data_terminator
                        @verify_data;

        last_chunk = "0"+ " "* CRLF $err{ err_ctx = "last_chunk"; };

        parser = chunk* last_chunk CRLF
                 $err{ err_ctx = "parser"; }
               ;

        main := parser @finalize;

    }%%

    %% write exec;

    ctx->parser_state = cs;

    *pos_addr = p;

    if (p != pe) {
        dd("ASSERTION FAILED: p != pe");
    }

    if (done) {
        return NGX_OK;
    }

    if (cs == chunked_error) {
        for (post.data = p, post.len = 0;
                post.data + post.len != pe; post.len++)
        {
            if (post.len >= POST_TEXT_LEN) {
                break;
            }
        }

        for (pre.data = p, pre.len = 0;
                pre.data != pos; pre.data--, pre.len++)
        {
            if (pre.len >= PRE_TEXT_LEN) {
                break;
            }
        }

        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                "bad chunked body (buf offset %O, total offset %uz, "
                "chunk size %uz, chunk bytes read %uz, "
                "total bytes to disk %uz, ctx \"%s\", "
                "raw body size %O), "
                "near \"%V <-- HERE %V\", marked by \" <-- HERE \").\n",
                (off_t) (p - pos), ctx->chunks_total_size,
                ctx->chunk_size, ctx->chunk_bytes_read,
                ctx->chunks_written_size, err_ctx,
                ctx->raw_body_size,
                &pre, &post);

        return NGX_ERROR;
    }

    return NGX_AGAIN;
}

