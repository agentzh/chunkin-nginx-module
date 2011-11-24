/* Copyright (C) agentzh */

#ifndef DDEBUG
#define DDEBUG 0
#endif

#include "ddebug.h"

#include "chunked_parser.h"
#include "ngx_http_chunkin_util.h"

#define ngx_chunkin_min(x, y) ((x) < (y) ? (x) : (y))

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
        ngx_http_chunkin_ctx_t *ctx, u_char **pos_addr, u_char *last, char *caller_info)
{
    int                 cs   = ctx->parser_state;
    ngx_connection_t    *c   = r->connection;
    signed char         *pos = (signed char *) *pos_addr;
    signed char         *p   = (signed char *) *pos_addr;
    signed char         *pe  = (signed char *) last;
    signed char         *eof = NULL;
    ngx_buf_t           *b;
    ngx_flag_t          done = 0;
    ngx_str_t           pre, post;
    char*               err_ctx = "";
    ngx_str_t           user_agent = ngx_null_string;
    ssize_t             rest;

    %%{
        #alphtype unsigned char;

        action finalize {
            done = 1;
        }

        action test_len {
            ctx->chunk_bytes_read < ctx->chunk_size
        }

        action read_data_byte {
            /* optimization for buffered chunk data */

            rest = ngx_chunkin_min(
                (ssize_t)ctx->chunk_size - (ssize_t)ctx->chunk_bytes_read,
                (ssize_t)(pe - p));

            dd("moving %d, chunk size %d, read %d, rest %d",
                (int)rest,
                (int)ctx->chunk_size,
                (int)ctx->chunk_bytes_read,
                (int)rest);

            ctx->chunk_bytes_read += rest;
            p += rest - 1;
            ctx->chunk->buf->last = (u_char *)p + 1;
            ctx->chunks_total_size += rest;

            /* dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size); */
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

            ctx->chunks_count++;

            if (ctx->chunks) {
                *ctx->next_chunk = ctx->chunk;
            } else {
                ctx->chunks = ctx->chunk;
            }

            ctx->next_chunk = &ctx->chunk->next;

            b = ctx->chunk->buf;

            b->last = b->pos = (u_char *) p;
            b->memory = 1;
        }

        action verify_data {
            if (ctx->chunk_bytes_read != ctx->chunk_size) {
                ngx_log_error(NGX_LOG_ERR, c->log, 0,
                        "ERROR: chunk size not met: "
                        "%uz != %uz\n", ctx->chunk_bytes_read,
                        ctx->chunk_size);
                *pos_addr = (u_char*) p;
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

        CR = "\r";

        LF = "\n";

        CRLF = CR LF $err{ err_ctx = "CRLF"; };

        chunk_size = (xdigit+ - "0"+) >start_reading_size $read_size;

        chunk_data_octet = any when test_len;

        chunk_data = (chunk_data_octet+)
                     >start_reading_data
                     $read_data_byte
                     $err{ err_ctx = "chunk_data"; }
                   ;

        chunk_data_terminator = CR when ! test_len LF
                                $err{ err_ctx = "chunk_data_terminator"; }
                              ;

        SP = ' ';
        HT = '\t';

        LWS = CRLF ? ( SP | HT )+;

        separator = "(" | ")" | "<" | ">" | "@"
                  | "," | ";" | ":" | "\\" | ["]
                  | "/" | "[" | "]" | "?" | "="
                  | "{" | "}" | SP | HT
                  ;

        CTL = 0..31 | 127;

        token = (any -- (CTL | separator))+;

        chunk_ext_name = token;

        TEXT = (any -- CTL) | LWS;

        qdtext = TEXT -- ["];

        CHAR = 0..127;

        quoted_pair = "\\" CHAR;

        quoted_string = ["] ( qdtext | quoted_pair )* ["];

        chunk_ext_val = token | quoted_string;

        chunk_extension = ( ";" LWS* chunk_ext_name LWS*
                            ("=" LWS* chunk_ext_val LWS*) ? )*
                        ;

        chunk = chunk_size (LWS* -- CRLF)
                    $err{ err_ctx = "chunk_size"; }
                    (chunk_extension -- CRLF) ?
                    CRLF
                    $err{ err_ctx = "chunk_ext"; }
                        chunk_data chunk_data_terminator
                        @verify_data;

        last_chunk = "0"+ (LWS* -- CRLF)
                      (chunk_extension -- CRLF) ?
                      CRLF $err{ err_ctx = "last_chunk"; }
                   ;

        parser = chunk* last_chunk CRLF
                 $err{ err_ctx = "parser"; }
               ;

        main := parser @finalize;

    }%%

    %% write exec;

    ctx->parser_state = cs;

    *pos_addr = (u_char *) p;

    if (p != pe) {
        dd("ASSERTION FAILED: p != pe");
    }

    if (done) {
        return NGX_OK;
    }

    if (cs == chunked_error) {

#if EXTENDED_DEBUG

        ngx_str_t           headers_buf, preread_buf;

#endif

        for (post.data = (u_char *) p, post.len = 0;
                post.data + post.len != (u_char *) pe; post.len++)
        {
            if (post.len >= POST_TEXT_LEN) {
                break;
            }
        }

        for (pre.data = (u_char *) p, pre.len = 0;
                pre.data != (u_char *) pos; pre.data--, pre.len++)
        {
            if (pre.len >= PRE_TEXT_LEN) {
                break;
            }
        }

        if (r->headers_in.user_agent) {
            user_agent = r->headers_in.user_agent->value;
        }

#if EXTENDED_DEBUG

        headers_buf.data = r->header_in->start;
        headers_buf.len = ctx->saved_header_in_pos - r->header_in->start;

        if (strcmp(caller_info, "preread") == 0) {
            preread_buf.data = (u_char *) pos;
            preread_buf.len = pe - pos;

        } else {
            preread_buf.data = ctx->saved_header_in_pos;
            preread_buf.len = r->header_in->pos - ctx->saved_header_in_pos;
        }

#endif

        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                "bad chunked body (buf size %O, buf offset %O, "
                "total decoded %uz, chunks count %d, "
                "chunk size %uz, chunk data read %uz, "
                "total to disk %uz, "
                "raw body size %O, caller \"%s\", "
                "plain_http %d, "

#if (NGX_HTTP_SSL)

                "ssl %d, "
#endif

                "keepalive %d, err ctx \"%s\", "
                "ctx ref count %ud, user agent \"%V\", "

#if EXTENDED_DEBUG

                "headers \"%V\", preread \"%V\", "

#endif

                "at char '%c' (%d), "
                "near \"%V <-- HERE %V\", marked by \" <-- HERE \").\n",
                (off_t) (pe - pos), (off_t) (p - pos),
                ctx->chunks_total_size, ctx->chunks_count,
                ctx->chunk_size, ctx->chunk_bytes_read,
                ctx->chunks_written_size,
                (off_t) ctx->raw_body_size, caller_info,
                (int) r->plain_http,

#if (NGX_HTTP_SSL)

                r->connection->ssl ? 1 : 0,

#endif

                (int) r->keepalive, err_ctx,
                ctx->count, &user_agent,

#if EXTENDED_DEBUG

                &headers_buf, &preread_buf,

#endif

                *p, *p,
                &pre, &post);

        return NGX_ERROR;
    }

    return NGX_AGAIN;
}

