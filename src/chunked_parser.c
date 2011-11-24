
#line 1 "src/chunked_parser.rl"
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


#line 20 "src/chunked_parser.rl"

#line 25 "src/chunked_parser.c"
static const int chunked_start = 1;
static const int chunked_first_final = 43;
static const int chunked_error = 0;

static const int chunked_en_main = 1;


#line 21 "src/chunked_parser.rl"


ngx_int_t
ngx_http_chunkin_init_chunked_parser(ngx_http_request_t *r,
        ngx_http_chunkin_ctx_t *ctx)
{
    int cs;

    
#line 43 "src/chunked_parser.c"
	{
	cs = chunked_start;
	}

#line 30 "src/chunked_parser.rl"

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

    
#line 236 "src/chunked_parser.rl"


    
#line 87 "src/chunked_parser.c"
	{
	short _widec;
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	if ( (*p) == 48 )
		goto tr0;
	if ( (*p) < 65 ) {
		if ( 49 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr2;
	} else
		goto tr2;
	goto st0;
tr3:
#line 227 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
	goto st0;
tr9:
#line 168 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 227 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
	goto st0;
tr11:
#line 168 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 227 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
#line 231 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	goto st0;
tr13:
#line 168 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 231 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	goto st0;
tr23:
#line 168 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
	goto st0;
tr29:
#line 218 "src/chunked_parser.rl"
	{ err_ctx = "chunk_size"; }
#line 221 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
	goto st0;
tr33:
#line 168 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 221 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
	goto st0;
tr35:
#line 168 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 221 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
#line 177 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	goto st0;
tr37:
#line 177 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	goto st0;
tr40:
#line 181 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data_terminator"; }
	goto st0;
tr43:
#line 221 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
	goto st0;
#line 166 "src/chunked_parser.c"
st0:
cs = 0;
	goto _out;
tr0:
#line 98 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
        }
#line 104 "src/chunked_parser.rl"
	{
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
	goto st2;
tr6:
#line 104 "src/chunked_parser.rl"
	{
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
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 214 "src/chunked_parser.c"
	switch( (*p) ) {
		case 9: goto st3;
		case 13: goto st4;
		case 32: goto st3;
		case 48: goto tr6;
		case 59: goto st7;
	}
	if ( (*p) < 65 ) {
		if ( 49 <= (*p) && (*p) <= 57 )
			goto tr7;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr7;
	} else
		goto tr7;
	goto tr3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	switch( (*p) ) {
		case 9: goto st3;
		case 13: goto st4;
		case 32: goto st3;
		case 59: goto st7;
	}
	goto tr3;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 10 )
		goto st5;
	goto tr9;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 13 )
		goto st6;
	goto tr11;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) == 10 )
		goto tr14;
	goto tr13;
tr14:
#line 66 "src/chunked_parser.rl"
	{
            done = 1;
        }
	goto st43;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
#line 273 "src/chunked_parser.c"
	goto tr13;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	switch( (*p) ) {
		case 9: goto st7;
		case 32: goto st7;
		case 34: goto st0;
		case 44: goto st0;
		case 47: goto st0;
		case 123: goto st0;
		case 125: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto st0;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto st0;
		} else if ( (*p) >= 58 )
			goto st0;
	} else
		goto st0;
	goto st8;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	switch( (*p) ) {
		case 9: goto st9;
		case 13: goto st4;
		case 32: goto st9;
		case 34: goto tr3;
		case 44: goto tr3;
		case 47: goto tr3;
		case 59: goto st7;
		case 61: goto st10;
		case 123: goto tr3;
		case 125: goto tr3;
		case 127: goto tr3;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto tr3;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto tr3;
		} else if ( (*p) >= 58 )
			goto tr3;
	} else
		goto tr3;
	goto st8;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	switch( (*p) ) {
		case 9: goto st9;
		case 13: goto st4;
		case 32: goto st9;
		case 59: goto st7;
		case 61: goto st10;
	}
	goto tr3;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	switch( (*p) ) {
		case 9: goto st10;
		case 32: goto st10;
		case 34: goto st12;
		case 44: goto st0;
		case 47: goto st0;
		case 123: goto st0;
		case 125: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto st0;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto st0;
		} else if ( (*p) >= 58 )
			goto st0;
	} else
		goto st0;
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
	switch( (*p) ) {
		case 9: goto st3;
		case 13: goto st4;
		case 32: goto st3;
		case 34: goto tr3;
		case 44: goto tr3;
		case 47: goto tr3;
		case 59: goto st7;
		case 123: goto tr3;
		case 125: goto tr3;
		case 127: goto tr3;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto tr3;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto tr3;
		} else if ( (*p) >= 58 )
			goto tr3;
	} else
		goto tr3;
	goto st11;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	switch( (*p) ) {
		case 34: goto st3;
		case 92: goto st13;
		case 127: goto st0;
	}
	if ( (*p) > 8 ) {
		if ( 10 <= (*p) && (*p) <= 31 )
			goto st0;
	} else if ( (*p) >= 0 )
		goto st0;
	goto st12;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	switch( (*p) ) {
		case 13: goto st14;
		case 34: goto st15;
		case 92: goto st13;
	}
	goto st12;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	switch( (*p) ) {
		case 34: goto st3;
		case 92: goto st13;
		case 127: goto tr23;
	}
	if ( (*p) > 8 ) {
		if ( 10 <= (*p) && (*p) <= 31 )
			goto tr23;
	} else if ( (*p) >= 0 )
		goto tr23;
	goto st12;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	switch( (*p) ) {
		case 9: goto st15;
		case 13: goto st4;
		case 32: goto st15;
		case 34: goto st3;
		case 59: goto st16;
		case 92: goto st13;
		case 127: goto tr3;
	}
	if ( 0 <= (*p) && (*p) <= 31 )
		goto tr3;
	goto st12;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	switch( (*p) ) {
		case 9: goto st16;
		case 32: goto st16;
		case 34: goto st3;
		case 44: goto st12;
		case 47: goto st12;
		case 92: goto st13;
		case 123: goto st12;
		case 125: goto st12;
		case 127: goto st0;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto st0;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto st12;
		} else if ( (*p) >= 58 )
			goto st12;
	} else
		goto st12;
	goto st17;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	switch( (*p) ) {
		case 9: goto st18;
		case 13: goto st4;
		case 32: goto st18;
		case 34: goto st3;
		case 44: goto st12;
		case 47: goto st12;
		case 59: goto st16;
		case 61: goto st19;
		case 92: goto st13;
		case 123: goto st12;
		case 125: goto st12;
		case 127: goto tr3;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto tr3;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto st12;
		} else if ( (*p) >= 58 )
			goto st12;
	} else
		goto st12;
	goto st17;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	switch( (*p) ) {
		case 9: goto st18;
		case 13: goto st4;
		case 32: goto st18;
		case 34: goto st3;
		case 59: goto st16;
		case 61: goto st19;
		case 92: goto st13;
		case 127: goto tr3;
	}
	if ( 0 <= (*p) && (*p) <= 31 )
		goto tr3;
	goto st12;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	switch( (*p) ) {
		case 9: goto st19;
		case 32: goto st19;
		case 34: goto st15;
		case 44: goto st12;
		case 47: goto st12;
		case 92: goto st13;
		case 123: goto st12;
		case 125: goto st12;
		case 127: goto st0;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto st0;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto st12;
		} else if ( (*p) >= 58 )
			goto st12;
	} else
		goto st12;
	goto st20;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	switch( (*p) ) {
		case 9: goto st15;
		case 13: goto st4;
		case 32: goto st15;
		case 34: goto st3;
		case 44: goto st12;
		case 47: goto st12;
		case 59: goto st16;
		case 92: goto st13;
		case 123: goto st12;
		case 125: goto st12;
		case 127: goto tr3;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto tr3;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto st12;
		} else if ( (*p) >= 58 )
			goto st12;
	} else
		goto st12;
	goto st20;
tr2:
#line 98 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
        }
#line 104 "src/chunked_parser.rl"
	{
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
	goto st21;
tr7:
#line 104 "src/chunked_parser.rl"
	{
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
	goto st21;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
#line 626 "src/chunked_parser.c"
	switch( (*p) ) {
		case 9: goto st22;
		case 13: goto st23;
		case 32: goto st22;
		case 59: goto st28;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr7;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr7;
	} else
		goto tr7;
	goto tr29;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	switch( (*p) ) {
		case 9: goto st22;
		case 13: goto st23;
		case 32: goto st22;
		case 59: goto st28;
	}
	goto tr29;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
	if ( (*p) == 10 )
		goto st24;
	goto tr33;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	_widec = (*p);
	_widec = (short)(128 + ((*p) - -128));
	if ( 
#line 70 "src/chunked_parser.rl"

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 256;
	if ( 384 <= _widec && _widec <= 639 )
		goto tr36;
	goto tr35;
tr36:
#line 119 "src/chunked_parser.rl"
	{
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
#line 74 "src/chunked_parser.rl"
	{
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
	goto st25;
tr39:
#line 74 "src/chunked_parser.rl"
	{
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
	goto st25;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
#line 749 "src/chunked_parser.c"
	_widec = (*p);
	if ( (*p) < 13 ) {
		if ( (*p) <= 12 ) {
			_widec = (short)(128 + ((*p) - -128));
			if ( 
#line 70 "src/chunked_parser.rl"

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 256;
		}
	} else if ( (*p) > 13 ) {
		if ( 14 <= (*p) )
 {			_widec = (short)(128 + ((*p) - -128));
			if ( 
#line 70 "src/chunked_parser.rl"

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 256;
		}
	} else {
		_widec = (short)(128 + ((*p) - -128));
		if ( 
#line 70 "src/chunked_parser.rl"

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 256;
	}
	if ( _widec == 269 )
		goto st26;
	if ( 384 <= _widec && _widec <= 639 )
		goto tr39;
	goto tr37;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) == 10 )
		goto tr41;
	goto tr40;
tr41:
#line 138 "src/chunked_parser.rl"
	{
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
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
#line 821 "src/chunked_parser.c"
	if ( (*p) == 48 )
		goto tr0;
	if ( (*p) < 65 ) {
		if ( 49 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr2;
	} else
		goto tr2;
	goto tr40;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
	switch( (*p) ) {
		case 9: goto st28;
		case 32: goto st28;
		case 34: goto st0;
		case 44: goto st0;
		case 47: goto st0;
		case 123: goto st0;
		case 125: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto st0;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto st0;
		} else if ( (*p) >= 58 )
			goto st0;
	} else
		goto st0;
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
	switch( (*p) ) {
		case 9: goto st30;
		case 13: goto st23;
		case 32: goto st30;
		case 34: goto tr43;
		case 44: goto tr43;
		case 47: goto tr43;
		case 59: goto st28;
		case 61: goto st31;
		case 123: goto tr43;
		case 125: goto tr43;
		case 127: goto tr43;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto tr43;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto tr43;
		} else if ( (*p) >= 58 )
			goto tr43;
	} else
		goto tr43;
	goto st29;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	switch( (*p) ) {
		case 9: goto st30;
		case 13: goto st23;
		case 32: goto st30;
		case 59: goto st28;
		case 61: goto st31;
	}
	goto tr43;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
	switch( (*p) ) {
		case 9: goto st31;
		case 32: goto st31;
		case 34: goto st34;
		case 44: goto st0;
		case 47: goto st0;
		case 123: goto st0;
		case 125: goto st0;
		case 127: goto st0;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto st0;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto st0;
		} else if ( (*p) >= 58 )
			goto st0;
	} else
		goto st0;
	goto st32;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
	switch( (*p) ) {
		case 9: goto st33;
		case 13: goto st23;
		case 32: goto st33;
		case 34: goto tr43;
		case 44: goto tr43;
		case 47: goto tr43;
		case 59: goto st28;
		case 123: goto tr43;
		case 125: goto tr43;
		case 127: goto tr43;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto tr43;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto tr43;
		} else if ( (*p) >= 58 )
			goto tr43;
	} else
		goto tr43;
	goto st32;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
	switch( (*p) ) {
		case 9: goto st33;
		case 13: goto st23;
		case 32: goto st33;
		case 59: goto st28;
	}
	goto tr43;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
	switch( (*p) ) {
		case 34: goto st33;
		case 92: goto st35;
		case 127: goto st0;
	}
	if ( (*p) > 8 ) {
		if ( 10 <= (*p) && (*p) <= 31 )
			goto st0;
	} else if ( (*p) >= 0 )
		goto st0;
	goto st34;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
	switch( (*p) ) {
		case 13: goto st36;
		case 34: goto st37;
		case 92: goto st35;
	}
	goto st34;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
	switch( (*p) ) {
		case 34: goto st33;
		case 92: goto st35;
		case 127: goto tr23;
	}
	if ( (*p) > 8 ) {
		if ( 10 <= (*p) && (*p) <= 31 )
			goto tr23;
	} else if ( (*p) >= 0 )
		goto tr23;
	goto st34;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
	switch( (*p) ) {
		case 9: goto st37;
		case 13: goto st23;
		case 32: goto st37;
		case 34: goto st33;
		case 59: goto st38;
		case 92: goto st35;
		case 127: goto tr43;
	}
	if ( 0 <= (*p) && (*p) <= 31 )
		goto tr43;
	goto st34;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
	switch( (*p) ) {
		case 9: goto st38;
		case 32: goto st38;
		case 34: goto st33;
		case 44: goto st34;
		case 47: goto st34;
		case 92: goto st35;
		case 123: goto st34;
		case 125: goto st34;
		case 127: goto st0;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto st0;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto st34;
		} else if ( (*p) >= 58 )
			goto st34;
	} else
		goto st34;
	goto st39;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
	switch( (*p) ) {
		case 9: goto st40;
		case 13: goto st23;
		case 32: goto st40;
		case 34: goto st33;
		case 44: goto st34;
		case 47: goto st34;
		case 59: goto st38;
		case 61: goto st41;
		case 92: goto st35;
		case 123: goto st34;
		case 125: goto st34;
		case 127: goto tr43;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto tr43;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto st34;
		} else if ( (*p) >= 58 )
			goto st34;
	} else
		goto st34;
	goto st39;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
	switch( (*p) ) {
		case 9: goto st40;
		case 13: goto st23;
		case 32: goto st40;
		case 34: goto st33;
		case 59: goto st38;
		case 61: goto st41;
		case 92: goto st35;
		case 127: goto tr43;
	}
	if ( 0 <= (*p) && (*p) <= 31 )
		goto tr43;
	goto st34;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
	switch( (*p) ) {
		case 9: goto st41;
		case 32: goto st41;
		case 34: goto st37;
		case 44: goto st34;
		case 47: goto st34;
		case 92: goto st35;
		case 123: goto st34;
		case 125: goto st34;
		case 127: goto st0;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto st0;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto st34;
		} else if ( (*p) >= 58 )
			goto st34;
	} else
		goto st34;
	goto st42;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
	switch( (*p) ) {
		case 9: goto st37;
		case 13: goto st23;
		case 32: goto st37;
		case 34: goto st33;
		case 44: goto st34;
		case 47: goto st34;
		case 59: goto st38;
		case 92: goto st35;
		case 123: goto st34;
		case 125: goto st34;
		case 127: goto tr43;
	}
	if ( (*p) < 40 ) {
		if ( 0 <= (*p) && (*p) <= 31 )
			goto tr43;
	} else if ( (*p) > 41 ) {
		if ( (*p) > 64 ) {
			if ( 91 <= (*p) && (*p) <= 93 )
				goto st34;
		} else if ( (*p) >= 58 )
			goto st34;
	} else
		goto st34;
	goto st42;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof43: cs = 43; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 
	_test_eof22: cs = 22; goto _test_eof; 
	_test_eof23: cs = 23; goto _test_eof; 
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 
	_test_eof26: cs = 26; goto _test_eof; 
	_test_eof27: cs = 27; goto _test_eof; 
	_test_eof28: cs = 28; goto _test_eof; 
	_test_eof29: cs = 29; goto _test_eof; 
	_test_eof30: cs = 30; goto _test_eof; 
	_test_eof31: cs = 31; goto _test_eof; 
	_test_eof32: cs = 32; goto _test_eof; 
	_test_eof33: cs = 33; goto _test_eof; 
	_test_eof34: cs = 34; goto _test_eof; 
	_test_eof35: cs = 35; goto _test_eof; 
	_test_eof36: cs = 36; goto _test_eof; 
	_test_eof37: cs = 37; goto _test_eof; 
	_test_eof38: cs = 38; goto _test_eof; 
	_test_eof39: cs = 39; goto _test_eof; 
	_test_eof40: cs = 40; goto _test_eof; 
	_test_eof41: cs = 41; goto _test_eof; 
	_test_eof42: cs = 42; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 14: 
	case 36: 
#line 168 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
	break;
	case 25: 
#line 177 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	break;
	case 26: 
	case 27: 
#line 181 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data_terminator"; }
	break;
	case 29: 
	case 30: 
	case 32: 
	case 33: 
	case 37: 
	case 39: 
	case 40: 
	case 42: 
#line 221 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
	break;
	case 2: 
	case 3: 
	case 8: 
	case 9: 
	case 11: 
	case 15: 
	case 17: 
	case 18: 
	case 20: 
#line 227 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
	break;
	case 23: 
#line 168 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 221 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
	break;
	case 4: 
#line 168 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 227 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
	break;
	case 6: 
#line 168 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 231 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	break;
	case 21: 
	case 22: 
#line 218 "src/chunked_parser.rl"
	{ err_ctx = "chunk_size"; }
#line 221 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
	break;
	case 24: 
#line 168 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 221 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
#line 177 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	break;
	case 5: 
#line 168 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 227 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
#line 231 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	break;
#line 1277 "src/chunked_parser.c"
	}
	}

	_out: {}
	}

#line 239 "src/chunked_parser.rl"

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

