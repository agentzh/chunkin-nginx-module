/* #line 1 "src/chunked_parser.rl" */
#define DDEBUG 0

#include "ddebug.h"

#include "chunked_parser.h"
#include "ngx_http_chunkin_util.h"

enum {
    PRE_TEXT_LEN = 15,
    POST_TEXT_LEN = 15
};

/* #line 14 "src/chunked_parser.rl" */

/* #line 17 "src/chunked_parser.c" */
static const int chunked_start = 1;
static const int chunked_first_final = 28;
static const int chunked_error = 0;

static const int chunked_en_main = 1;

/* #line 15 "src/chunked_parser.rl" */

ngx_int_t
ngx_http_chunkin_init_chunked_parser(ngx_http_request_t *r,
        ngx_http_chunkin_ctx_t *ctx)
{
    int cs;

    
/* #line 33 "src/chunked_parser.c" */
	{
	cs = chunked_start;
	}
/* #line 23 "src/chunked_parser.rl" */

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
    ngx_buf_t           *b;
    ngx_flag_t          done = 0;
    ngx_str_t           pre, post;

    /* #line 161 "src/chunked_parser.rl" */


    
/* #line 70 "src/chunked_parser.c" */
	{
	int _widec;
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
tr15:
/* #line 113 "src/chunked_parser.rl" */
	{
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
	goto st1;
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
/* #line 109 "src/chunked_parser.c" */
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
st0:
cs = 0;
	goto _out;
tr0:
/* #line 75 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
        }
/* #line 81 "src/chunked_parser.rl" */
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
/* #line 151 "src/chunked_parser.c" */
	switch( (*p) ) {
		case 13: goto st3;
		case 32: goto st6;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr5;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr5;
	} else
		goto tr5;
	goto st0;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) == 10 )
		goto st4;
	goto st0;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 13 )
		goto st5;
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 10 )
		goto tr8;
	goto st0;
tr8:
/* #line 54 "src/chunked_parser.rl" */
	{
            done = 1;
        }
	goto st28;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
/* #line 196 "src/chunked_parser.c" */
	goto st0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	switch( (*p) ) {
		case 13: goto st3;
		case 32: goto st6;
	}
	goto st0;
tr2:
/* #line 75 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
        }
/* #line 81 "src/chunked_parser.rl" */
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
	goto st7;
tr5:
/* #line 81 "src/chunked_parser.rl" */
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
	goto st7;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
/* #line 251 "src/chunked_parser.c" */
	switch( (*p) ) {
		case 13: goto st8;
		case 32: goto st24;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr5;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr5;
	} else
		goto tr5;
	goto st0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	if ( (*p) == 10 )
		goto st9;
	goto st0;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
/* #line 58 "src/chunked_parser.rl" */

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	switch( _widec ) {
		case 65549: goto tr12;
		case 131085: goto tr14;
	}
	if ( 98304 <= _widec && _widec <= 163839 )
		goto tr13;
	goto st0;
tr12:
/* #line 96 "src/chunked_parser.rl" */
	{
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
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
/* #line 313 "src/chunked_parser.c" */
	if ( (*p) == 10 )
		goto tr15;
	goto st0;
tr13:
/* #line 96 "src/chunked_parser.rl" */
	{
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
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	goto st11;
tr17:
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
/* #line 368 "src/chunked_parser.c" */
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
/* #line 58 "src/chunked_parser.rl" */

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	switch( _widec ) {
		case 65549: goto st10;
		case 131085: goto tr18;
	}
	if ( 98304 <= _widec && _widec <= 163839 )
		goto tr17;
	goto st0;
tr14:
/* #line 96 "src/chunked_parser.rl" */
	{
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
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	goto st12;
tr18:
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
/* #line 434 "src/chunked_parser.c" */
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
/* #line 58 "src/chunked_parser.rl" */

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	switch( _widec ) {
		case 65546: goto tr15;
		case 65549: goto st10;
		case 131082: goto tr19;
		case 131085: goto tr18;
	}
	if ( 98304 <= _widec && _widec <= 163839 )
		goto tr17;
	goto st0;
tr19:
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
/* #line 113 "src/chunked_parser.rl" */
	{
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
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
/* #line 496 "src/chunked_parser.c" */
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
/* #line 58 "src/chunked_parser.rl" */

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	switch( _widec ) {
		case 65549: goto st10;
		case 65584: goto tr0;
		case 131085: goto tr18;
		case 131120: goto tr20;
	}
	if ( _widec < 131121 ) {
		if ( _widec < 65601 ) {
			if ( 65585 <= _widec && _widec <= 65593 )
				goto tr2;
		} else if ( _widec > 65606 ) {
			if ( _widec > 65638 ) {
				if ( 98304 <= _widec && _widec <= 131119 )
					goto tr17;
			} else if ( _widec >= 65633 )
				goto tr2;
		} else
			goto tr2;
	} else if ( _widec > 131129 ) {
		if ( _widec < 131143 ) {
			if ( _widec > 131136 ) {
				if ( 131137 <= _widec && _widec <= 131142 )
					goto tr21;
			} else if ( _widec >= 131130 )
				goto tr17;
		} else if ( _widec > 131168 ) {
			if ( _widec > 131174 ) {
				if ( 131175 <= _widec && _widec <= 163839 )
					goto tr17;
			} else if ( _widec >= 131169 )
				goto tr21;
		} else
			goto tr17;
	} else
		goto tr21;
	goto st0;
tr20:
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
/* #line 75 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
        }
/* #line 81 "src/chunked_parser.rl" */
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
	goto st14;
tr36:
/* #line 96 "src/chunked_parser.rl" */
	{
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
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
/* #line 75 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
        }
/* #line 81 "src/chunked_parser.rl" */
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
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
/* #line 633 "src/chunked_parser.c" */
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
/* #line 58 "src/chunked_parser.rl" */

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	switch( _widec ) {
		case 65549: goto st15;
		case 65568: goto st6;
		case 131085: goto tr23;
		case 131104: goto tr24;
	}
	if ( _widec < 131120 ) {
		if ( _widec < 65601 ) {
			if ( 65584 <= _widec && _widec <= 65593 )
				goto tr5;
		} else if ( _widec > 65606 ) {
			if ( _widec > 65638 ) {
				if ( 98304 <= _widec && _widec <= 131119 )
					goto tr17;
			} else if ( _widec >= 65633 )
				goto tr5;
		} else
			goto tr5;
	} else if ( _widec > 131129 ) {
		if ( _widec < 131143 ) {
			if ( _widec > 131136 ) {
				if ( 131137 <= _widec && _widec <= 131142 )
					goto tr25;
			} else if ( _widec >= 131130 )
				goto tr17;
		} else if ( _widec > 131168 ) {
			if ( _widec > 131174 ) {
				if ( 131175 <= _widec && _widec <= 163839 )
					goto tr17;
			} else if ( _widec >= 131169 )
				goto tr25;
		} else
			goto tr17;
	} else
		goto tr25;
	goto st0;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	if ( (*p) == 10 )
		goto tr26;
	goto st0;
tr26:
/* #line 113 "src/chunked_parser.rl" */
	{
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
	goto st16;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
/* #line 716 "src/chunked_parser.c" */
	switch( (*p) ) {
		case 13: goto st5;
		case 48: goto tr0;
	}
	if ( (*p) < 65 ) {
		if ( 49 <= (*p) && (*p) <= 57 )
			goto tr2;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr2;
	} else
		goto tr2;
	goto st0;
tr23:
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	goto st17;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
/* #line 749 "src/chunked_parser.c" */
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
/* #line 58 "src/chunked_parser.rl" */

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	switch( _widec ) {
		case 65546: goto tr26;
		case 65549: goto st10;
		case 131082: goto tr27;
		case 131085: goto tr18;
	}
	if ( 98304 <= _widec && _widec <= 163839 )
		goto tr17;
	goto st0;
tr27:
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
/* #line 113 "src/chunked_parser.rl" */
	{
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
	goto st18;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
/* #line 811 "src/chunked_parser.c" */
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
/* #line 58 "src/chunked_parser.rl" */

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	switch( _widec ) {
		case 65549: goto st19;
		case 65584: goto tr0;
		case 131085: goto tr29;
		case 131120: goto tr20;
	}
	if ( _widec < 131121 ) {
		if ( _widec < 65601 ) {
			if ( 65585 <= _widec && _widec <= 65593 )
				goto tr2;
		} else if ( _widec > 65606 ) {
			if ( _widec > 65638 ) {
				if ( 98304 <= _widec && _widec <= 131119 )
					goto tr17;
			} else if ( _widec >= 65633 )
				goto tr2;
		} else
			goto tr2;
	} else if ( _widec > 131129 ) {
		if ( _widec < 131143 ) {
			if ( _widec > 131136 ) {
				if ( 131137 <= _widec && _widec <= 131142 )
					goto tr21;
			} else if ( _widec >= 131130 )
				goto tr17;
		} else if ( _widec > 131168 ) {
			if ( _widec > 131174 ) {
				if ( 131175 <= _widec && _widec <= 163839 )
					goto tr17;
			} else if ( _widec >= 131169 )
				goto tr21;
		} else
			goto tr17;
	} else
		goto tr21;
	goto st0;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	if ( (*p) == 10 )
		goto tr30;
	goto st0;
tr30:
/* #line 113 "src/chunked_parser.rl" */
	{
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
/* #line 54 "src/chunked_parser.rl" */
	{
            done = 1;
        }
	goto st29;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
/* #line 898 "src/chunked_parser.c" */
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
tr29:
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	goto st20;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
/* #line 929 "src/chunked_parser.c" */
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
/* #line 58 "src/chunked_parser.rl" */

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	switch( _widec ) {
		case 65546: goto tr30;
		case 65549: goto st10;
		case 131082: goto tr31;
		case 131085: goto tr18;
	}
	if ( 98304 <= _widec && _widec <= 163839 )
		goto tr17;
	goto st0;
tr31:
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
/* #line 113 "src/chunked_parser.rl" */
	{
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
/* #line 54 "src/chunked_parser.rl" */
	{
            done = 1;
        }
	goto st30;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
/* #line 995 "src/chunked_parser.c" */
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
/* #line 58 "src/chunked_parser.rl" */

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	switch( _widec ) {
		case 65549: goto st10;
		case 65584: goto tr0;
		case 131085: goto tr18;
		case 131120: goto tr20;
	}
	if ( _widec < 131121 ) {
		if ( _widec < 65601 ) {
			if ( 65585 <= _widec && _widec <= 65593 )
				goto tr2;
		} else if ( _widec > 65606 ) {
			if ( _widec > 65638 ) {
				if ( 98304 <= _widec && _widec <= 131119 )
					goto tr17;
			} else if ( _widec >= 65633 )
				goto tr2;
		} else
			goto tr2;
	} else if ( _widec > 131129 ) {
		if ( _widec < 131143 ) {
			if ( _widec > 131136 ) {
				if ( 131137 <= _widec && _widec <= 131142 )
					goto tr21;
			} else if ( _widec >= 131130 )
				goto tr17;
		} else if ( _widec > 131168 ) {
			if ( _widec > 131174 ) {
				if ( 131175 <= _widec && _widec <= 163839 )
					goto tr17;
			} else if ( _widec >= 131169 )
				goto tr21;
		} else
			goto tr17;
	} else
		goto tr21;
	goto st0;
tr21:
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
/* #line 75 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
        }
/* #line 81 "src/chunked_parser.rl" */
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
tr25:
/* #line 81 "src/chunked_parser.rl" */
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
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	goto st21;
tr37:
/* #line 96 "src/chunked_parser.rl" */
	{
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
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
/* #line 75 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
        }
/* #line 81 "src/chunked_parser.rl" */
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
/* #line 1162 "src/chunked_parser.c" */
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
/* #line 58 "src/chunked_parser.rl" */

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	switch( _widec ) {
		case 65549: goto st22;
		case 65568: goto st24;
		case 131085: goto tr33;
		case 131104: goto tr34;
	}
	if ( _widec < 131120 ) {
		if ( _widec < 65601 ) {
			if ( 65584 <= _widec && _widec <= 65593 )
				goto tr5;
		} else if ( _widec > 65606 ) {
			if ( _widec > 65638 ) {
				if ( 98304 <= _widec && _widec <= 131119 )
					goto tr17;
			} else if ( _widec >= 65633 )
				goto tr5;
		} else
			goto tr5;
	} else if ( _widec > 131129 ) {
		if ( _widec < 131143 ) {
			if ( _widec > 131136 ) {
				if ( 131137 <= _widec && _widec <= 131142 )
					goto tr25;
			} else if ( _widec >= 131130 )
				goto tr17;
		} else if ( _widec > 131168 ) {
			if ( _widec > 131174 ) {
				if ( 131175 <= _widec && _widec <= 163839 )
					goto tr17;
			} else if ( _widec >= 131169 )
				goto tr25;
		} else
			goto tr17;
	} else
		goto tr25;
	goto st0;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
	if ( (*p) == 10 )
		goto tr35;
	goto st0;
tr35:
/* #line 113 "src/chunked_parser.rl" */
	{
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
	goto st23;
tr38:
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
/* #line 113 "src/chunked_parser.rl" */
	{
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
	goto st23;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
/* #line 1286 "src/chunked_parser.c" */
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
/* #line 58 "src/chunked_parser.rl" */

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	switch( _widec ) {
		case 65549: goto tr12;
		case 65584: goto tr0;
		case 131085: goto tr14;
		case 131120: goto tr36;
	}
	if ( _widec < 131121 ) {
		if ( _widec < 65601 ) {
			if ( 65585 <= _widec && _widec <= 65593 )
				goto tr2;
		} else if ( _widec > 65606 ) {
			if ( _widec > 65638 ) {
				if ( 98304 <= _widec && _widec <= 131119 )
					goto tr13;
			} else if ( _widec >= 65633 )
				goto tr2;
		} else
			goto tr2;
	} else if ( _widec > 131129 ) {
		if ( _widec < 131143 ) {
			if ( _widec > 131136 ) {
				if ( 131137 <= _widec && _widec <= 131142 )
					goto tr37;
			} else if ( _widec >= 131130 )
				goto tr13;
		} else if ( _widec > 131168 ) {
			if ( _widec > 131174 ) {
				if ( 131175 <= _widec && _widec <= 163839 )
					goto tr13;
			} else if ( _widec >= 131169 )
				goto tr37;
		} else
			goto tr13;
	} else
		goto tr37;
	goto st0;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	switch( (*p) ) {
		case 13: goto st8;
		case 32: goto st24;
	}
	goto st0;
tr33:
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	goto st25;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
/* #line 1358 "src/chunked_parser.c" */
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
/* #line 58 "src/chunked_parser.rl" */

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	switch( _widec ) {
		case 65546: goto tr35;
		case 65549: goto st10;
		case 131082: goto tr38;
		case 131085: goto tr18;
	}
	if ( 98304 <= _widec && _widec <= 163839 )
		goto tr17;
	goto st0;
tr34:
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	goto st26;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
/* #line 1394 "src/chunked_parser.c" */
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
/* #line 58 "src/chunked_parser.rl" */

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	switch( _widec ) {
		case 65549: goto st22;
		case 65568: goto st24;
		case 131085: goto tr33;
		case 131104: goto tr34;
	}
	if ( 98304 <= _widec && _widec <= 163839 )
		goto tr17;
	goto st0;
tr24:
/* #line 62 "src/chunked_parser.rl" */
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	goto st27;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
/* #line 1430 "src/chunked_parser.c" */
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
/* #line 58 "src/chunked_parser.rl" */

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	switch( _widec ) {
		case 65549: goto st15;
		case 65568: goto st6;
		case 131085: goto tr23;
		case 131104: goto tr24;
	}
	if ( 98304 <= _widec && _widec <= 163839 )
		goto tr17;
	goto st0;
	}
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof28: cs = 28; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
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
	_test_eof29: cs = 29; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof30: cs = 30; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 
	_test_eof22: cs = 22; goto _test_eof; 
	_test_eof23: cs = 23; goto _test_eof; 
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 
	_test_eof26: cs = 26; goto _test_eof; 
	_test_eof27: cs = 27; goto _test_eof; 

	_test_eof: {}
	_out: {}
	}
/* #line 164 "src/chunked_parser.rl" */

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
                "bad chunked body (offset %O, near \"%V <-- HERE %V\", marked by \" <-- HERE \").\n",
                (off_t) (p - pos), &pre, &post);

        return NGX_ERROR;
    }

    return NGX_AGAIN;
}

