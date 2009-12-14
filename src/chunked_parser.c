
#line 1 "src/chunked_parser.rl"
#define DDEBUG 0

#include "ddebug.h"

#include "chunked_parser.h"
#include "ngx_http_chunkin_util.h"

enum {
    PRE_TEXT_LEN = 25,
    POST_TEXT_LEN = 25
};


#line 14 "src/chunked_parser.rl"

#line 19 "src/chunked_parser.c"
static const int chunked_start = 1;
static const int chunked_first_final = 14;
static const int chunked_error = 0;

static const int chunked_en_main = 1;


#line 15 "src/chunked_parser.rl"


ngx_int_t
ngx_http_chunkin_init_chunked_parser(ngx_http_request_t *r,
        ngx_http_chunkin_ctx_t *ctx)
{
    int cs;

    
#line 37 "src/chunked_parser.c"
	{
	cs = chunked_start;
	}

#line 24 "src/chunked_parser.rl"

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
    u_char              *pos = *pos_addr;
    u_char              *p   = *pos_addr;
    u_char              *pe  = last;
    u_char              *eof = NULL;
    ngx_buf_t           *b;
    ngx_flag_t          done = 0;
    ngx_str_t           pre, post;
    char*               err_ctx = "";
    ngx_str_t           user_agent = ngx_string("");

    
#line 177 "src/chunked_parser.rl"


    
#line 80 "src/chunked_parser.c"
	{
	int _widec;
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
st1:
	if ( ++p == pe )
		goto _test_eof1;
case 1:
	switch( (*p) ) {
		case 13: goto st1;
		case 32: goto st1;
		case 48: goto tr2;
	}
	if ( (*p) < 49 ) {
		if ( 9 <= (*p) && (*p) <= 10 )
			goto st1;
	} else if ( (*p) > 57 ) {
		if ( (*p) > 70 ) {
			if ( 97 <= (*p) && (*p) <= 102 )
				goto tr3;
		} else if ( (*p) >= 65 )
			goto tr3;
	} else
		goto tr3;
	goto st0;
tr4:
#line 144 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 169 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
	goto st0;
tr10:
#line 144 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 169 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
#line 172 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	goto st0;
tr12:
#line 144 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 172 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	goto st0;
tr14:
#line 144 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 165 "src/chunked_parser.rl"
	{ err_ctx = "chunk_size"; }
	goto st0;
tr18:
#line 144 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 165 "src/chunked_parser.rl"
	{ err_ctx = "chunk_size"; }
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	goto st0;
tr20:
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	goto st0;
tr23:
#line 157 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data_terminator"; }
	goto st0;
#line 150 "src/chunked_parser.c"
st0:
cs = 0;
	goto _out;
tr2:
#line 80 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
        }
#line 86 "src/chunked_parser.rl"
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
tr7:
#line 86 "src/chunked_parser.rl"
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
#line 198 "src/chunked_parser.c"
	switch( (*p) ) {
		case 13: goto st3;
		case 32: goto st6;
		case 48: goto tr7;
	}
	if ( (*p) < 65 ) {
		if ( 49 <= (*p) && (*p) <= 57 )
			goto tr8;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr8;
	} else
		goto tr8;
	goto tr4;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
	if ( (*p) == 10 )
		goto st4;
	goto tr4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
	if ( (*p) == 13 )
		goto st5;
	goto tr10;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 10 )
		goto tr13;
	goto tr12;
tr13:
#line 59 "src/chunked_parser.rl"
	{
            done = 1;
        }
	goto st14;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
#line 244 "src/chunked_parser.c"
	goto tr12;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	switch( (*p) ) {
		case 13: goto st3;
		case 32: goto st6;
	}
	goto tr4;
tr3:
#line 80 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
        }
#line 86 "src/chunked_parser.rl"
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
tr8:
#line 86 "src/chunked_parser.rl"
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
#line 299 "src/chunked_parser.c"
	switch( (*p) ) {
		case 9: goto st8;
		case 13: goto st9;
		case 32: goto st8;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 57 )
			goto tr8;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr8;
	} else
		goto tr8;
	goto tr14;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	switch( (*p) ) {
		case 9: goto st8;
		case 13: goto st9;
		case 32: goto st8;
	}
	goto tr14;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
	if ( (*p) == 10 )
		goto st10;
	goto tr14;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
	_widec = (*p);
	_widec = (int)(32768 + ((*p) - -32768));
	if ( 
#line 63 "src/chunked_parser.rl"

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	if ( 98304 <= _widec && _widec <= 163839 )
		goto tr19;
	goto tr18;
tr19:
#line 101 "src/chunked_parser.rl"
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
#line 67 "src/chunked_parser.rl"
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
tr22:
#line 67 "src/chunked_parser.rl"
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
#line 396 "src/chunked_parser.c"
	_widec = (*p);
	if ( (*p) < 13 ) {
		if ( (*p) <= 12 ) {
			_widec = (int)(32768 + ((*p) - -32768));
			if ( 
#line 63 "src/chunked_parser.rl"

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
		}
	} else if ( (*p) > 13 ) {
		if ( 14 <= (*p) )
 {			_widec = (int)(32768 + ((*p) - -32768));
			if ( 
#line 63 "src/chunked_parser.rl"

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
		}
	} else {
		_widec = (int)(32768 + ((*p) - -32768));
		if ( 
#line 63 "src/chunked_parser.rl"

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 65536;
	}
	if ( _widec == 65549 )
		goto st12;
	if ( 98304 <= _widec && _widec <= 163839 )
		goto tr22;
	goto tr20;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
	if ( (*p) == 10 )
		goto tr24;
	goto tr23;
tr24:
#line 118 "src/chunked_parser.rl"
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
#line 468 "src/chunked_parser.c"
	if ( (*p) == 48 )
		goto tr2;
	if ( (*p) < 65 ) {
		if ( 49 <= (*p) && (*p) <= 57 )
			goto tr3;
	} else if ( (*p) > 70 ) {
		if ( 97 <= (*p) && (*p) <= 102 )
			goto tr3;
	} else
		goto tr3;
	goto tr23;
	}
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 11: 
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	break;
	case 12: 
	case 13: 
#line 157 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data_terminator"; }
	break;
	case 7: 
	case 8: 
	case 9: 
#line 144 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 165 "src/chunked_parser.rl"
	{ err_ctx = "chunk_size"; }
	break;
	case 2: 
	case 3: 
	case 6: 
#line 144 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 169 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
	break;
	case 5: 
#line 144 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 172 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	break;
	case 10: 
#line 144 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 165 "src/chunked_parser.rl"
	{ err_ctx = "chunk_size"; }
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	break;
	case 4: 
#line 144 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 169 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
#line 172 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	break;
#line 547 "src/chunked_parser.c"
	}
	}

	_out: {}
	}

#line 180 "src/chunked_parser.rl"

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

        if (r->headers_in.user_agent) {
            user_agent = r->headers_in.user_agent->value;
        }

        ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                "bad chunked body (buf size %O, buf offset %O, "
                "total decoded %uz, chunks count %d, "
                "chunk size %uz, chunk data read %uz, "
                "total to disk %uz, "
                "raw body size %O, caller \"%s\", "
                "keepalive %d, err ctx \"%s\", "
                "ctx ref count %ud, user agent \"%V\", "
                "at char '%c' (%d), "
                "near \"%V <-- HERE %V\", marked by \" <-- HERE \").\n",
                (off_t) (pe - pos), (off_t) (p - pos),
                ctx->chunks_total_size, ctx->chunks_count,
                ctx->chunk_size, ctx->chunk_bytes_read,
                ctx->chunks_written_size,
                (off_t) ctx->raw_body_size, caller_info,
                (int) r->keepalive, err_ctx,
                ctx->count, &user_agent,
                *p, *p,
                &pre, &post);

        return NGX_ERROR;
    }

    return NGX_AGAIN;
}

