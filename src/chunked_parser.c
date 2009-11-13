#line 1 "src/chunked_parser.rl"
#define DDEBUG 0

#include "ddebug.h"

#include "chunked_parser.h"

#line 8 "src/chunked_parser.rl"

#line 11 "src/chunked_parser.c"
static const int chunked_start = 1;
static const int chunked_first_final = 15;
static const int chunked_error = 0;

static const int chunked_en_main = 1;

#line 9 "src/chunked_parser.rl"

ngx_int_t
ngx_http_chunkin_init_chunked_parser(ngx_http_request_t *r,
        ngx_http_chunkin_ctx_t *ctx)
{
    int cs;

    
#line 27 "src/chunked_parser.c"
	{
	cs = chunked_start;
	}
#line 17 "src/chunked_parser.rl"

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

    #line 157 "src/chunked_parser.rl"


    
#line 62 "src/chunked_parser.c"
	{
	short _widec;
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
st1:
	if ( ++p == pe )
		goto _test_eof1;
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
tr7:
#line 44 "src/chunked_parser.rl"
	{
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                    "bad chunked body (offset %O).\n", p - pos);
        }
	goto st0;
tr13:
#line 54 "src/chunked_parser.rl"
	{
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                "bad chunk data "
                "(bytes already read %uz, bytes expected: %uz): "
                "offset %O.\n", ctx->chunk_bytes_read,
                ctx->chunk_size, p - pos);
        }
	goto st0;
tr22:
#line 49 "src/chunked_parser.rl"
	{
            ctx->parser_state = chunked_first_final;
            return NGX_OK;
        }
	goto st0;
#line 108 "src/chunked_parser.c"
st0:
cs = 0;
	goto _out;
tr0:
#line 78 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
        }
#line 84 "src/chunked_parser.rl"
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
#line 139 "src/chunked_parser.c"
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
	goto tr7;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 10 )
		goto st15;
	goto tr7;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	goto tr22;
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
#line 78 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
        }
#line 84 "src/chunked_parser.rl"
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
#line 84 "src/chunked_parser.rl"
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
#line 232 "src/chunked_parser.c"
	switch( (*p) ) {
		case 13: goto st8;
		case 32: goto st14;
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
	_widec = (short)(128 + ((*p) - -128));
	if ( 
#line 62 "src/chunked_parser.rl"

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 256;
	switch( _widec ) {
		case 269: goto tr14;
		case 525: goto tr16;
	}
	if ( 384 <= _widec && _widec <= 639 )
		goto tr15;
	goto tr13;
tr14:
#line 99 "src/chunked_parser.rl"
	{
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
                ctx->next_chunk = &ctx->chunks->next;
            } else {
                *ctx->next_chunk = ctx->chunk;
                ctx->next_chunk = &ctx->chunk->next;
            }
            b->end = b->last = b->pos = b->start = p;
            b->memory = 1;
        }
#line 126 "src/chunked_parser.rl"
	{
            if (ctx->chunk_bytes_read != ctx->chunk_size) {
                ngx_log_error(NGX_LOG_ERR, c->log, 0,
                        "ERROR: chunk size not meet: "
                        "%uz != %uz\n", ctx->chunk_bytes_read,
                        ctx->chunk_size);
                {p++; cs = 10; goto _out;}
            }
        }
	goto st10;
tr18:
#line 126 "src/chunked_parser.rl"
	{
            if (ctx->chunk_bytes_read != ctx->chunk_size) {
                ngx_log_error(NGX_LOG_ERR, c->log, 0,
                        "ERROR: chunk size not meet: "
                        "%uz != %uz\n", ctx->chunk_bytes_read,
                        ctx->chunk_size);
                {p++; cs = 10; goto _out;}
            }
        }
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
#line 326 "src/chunked_parser.c"
	if ( (*p) == 10 )
		goto st1;
	goto st0;
tr15:
#line 99 "src/chunked_parser.rl"
	{
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
                ctx->next_chunk = &ctx->chunks->next;
            } else {
                *ctx->next_chunk = ctx->chunk;
                ctx->next_chunk = &ctx->chunk->next;
            }
            b->end = b->last = b->pos = b->start = p;
            b->memory = 1;
        }
#line 66 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read++;
            ctx->chunk->buf->last++;
            ctx->chunk->buf->end++;
            ctx->chunks_total_size++;

            dd("bytes read: %d", ctx->chunk->buf->last - ctx->chunk->buf->pos);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	goto st11;
tr19:
#line 66 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read++;
            ctx->chunk->buf->last++;
            ctx->chunk->buf->end++;
            ctx->chunks_total_size++;

            dd("bytes read: %d", ctx->chunk->buf->last - ctx->chunk->buf->pos);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
#line 389 "src/chunked_parser.c"
	_widec = (*p);
	_widec = (short)(128 + ((*p) - -128));
	if ( 
#line 62 "src/chunked_parser.rl"

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 256;
	switch( _widec ) {
		case 269: goto tr18;
		case 525: goto tr20;
	}
	if ( 384 <= _widec && _widec <= 639 )
		goto tr19;
	goto tr13;
tr16:
#line 99 "src/chunked_parser.rl"
	{
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
                ctx->next_chunk = &ctx->chunks->next;
            } else {
                *ctx->next_chunk = ctx->chunk;
                ctx->next_chunk = &ctx->chunk->next;
            }
            b->end = b->last = b->pos = b->start = p;
            b->memory = 1;
        }
#line 66 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read++;
            ctx->chunk->buf->last++;
            ctx->chunk->buf->end++;
            ctx->chunks_total_size++;

            dd("bytes read: %d", ctx->chunk->buf->last - ctx->chunk->buf->pos);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
#line 126 "src/chunked_parser.rl"
	{
            if (ctx->chunk_bytes_read != ctx->chunk_size) {
                ngx_log_error(NGX_LOG_ERR, c->log, 0,
                        "ERROR: chunk size not meet: "
                        "%uz != %uz\n", ctx->chunk_bytes_read,
                        ctx->chunk_size);
                {p++; cs = 12; goto _out;}
            }
        }
	goto st12;
tr20:
#line 66 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read++;
            ctx->chunk->buf->last++;
            ctx->chunk->buf->end++;
            ctx->chunks_total_size++;

            dd("bytes read: %d", ctx->chunk->buf->last - ctx->chunk->buf->pos);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
#line 126 "src/chunked_parser.rl"
	{
            if (ctx->chunk_bytes_read != ctx->chunk_size) {
                ngx_log_error(NGX_LOG_ERR, c->log, 0,
                        "ERROR: chunk size not meet: "
                        "%uz != %uz\n", ctx->chunk_bytes_read,
                        ctx->chunk_size);
                {p++; cs = 12; goto _out;}
            }
        }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
#line 483 "src/chunked_parser.c"
	_widec = (*p);
	_widec = (short)(128 + ((*p) - -128));
	if ( 
#line 62 "src/chunked_parser.rl"

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 256;
	switch( _widec ) {
		case 266: goto st1;
		case 269: goto tr18;
		case 522: goto tr21;
		case 525: goto tr20;
	}
	if ( 384 <= _widec && _widec <= 639 )
		goto tr19;
	goto tr13;
tr21:
#line 66 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read++;
            ctx->chunk->buf->last++;
            ctx->chunk->buf->end++;
            ctx->chunks_total_size++;

            dd("bytes read: %d", ctx->chunk->buf->last - ctx->chunk->buf->pos);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	goto st13;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
#line 518 "src/chunked_parser.c"
	_widec = (*p);
	_widec = (short)(128 + ((*p) - -128));
	if ( 
#line 62 "src/chunked_parser.rl"

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 256;
	switch( _widec ) {
		case 269: goto tr18;
		case 304: goto tr0;
		case 525: goto tr20;
	}
	if ( _widec < 321 ) {
		if ( 305 <= _widec && _widec <= 313 )
			goto tr2;
	} else if ( _widec > 326 ) {
		if ( _widec > 358 ) {
			if ( 384 <= _widec && _widec <= 639 )
				goto tr19;
		} else if ( _widec >= 353 )
			goto tr2;
	} else
		goto tr2;
	goto tr13;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	switch( (*p) ) {
		case 13: goto st8;
		case 32: goto st14;
	}
	goto st0;
	}
	_test_eof1: cs = 1; goto _test_eof; 
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 4: 
	case 5: 
#line 44 "src/chunked_parser.rl"
	{
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                    "bad chunked body (offset %O).\n", p - pos);
        }
	break;
	case 9: 
	case 11: 
	case 12: 
	case 13: 
#line 54 "src/chunked_parser.rl"
	{
            ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
                "bad chunk data "
                "(bytes already read %uz, bytes expected: %uz): "
                "offset %O.\n", ctx->chunk_bytes_read,
                ctx->chunk_size, p - pos);
        }
	break;
#line 594 "src/chunked_parser.c"
	}
	}

	_out: {}
	}
#line 160 "src/chunked_parser.rl"

    ctx->parser_state = cs;

    if (cs >= chunked_first_final) {
        return NGX_OK;
    }

    if (cs == chunked_error) {
        return NGX_ERROR;
    }

    return NGX_AGAIN;
}

