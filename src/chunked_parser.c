
#line 1 "src/chunked_parser.rl"
/* Copyright (C) agentzh */

#define EXTENDED_DEBUG 1
#define DDEBUG 0

#include "ddebug.h"

#include "chunked_parser.h"
#include "ngx_http_chunkin_util.h"

enum {
    PRE_TEXT_LEN = 25,
    POST_TEXT_LEN = 25
};


#line 17 "src/chunked_parser.rl"

#line 22 "src/chunked_parser.c"
static const char _chunked_cond_offsets[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 1, 4, 4, 4
};

static const char _chunked_cond_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 3, 0, 0, 0
};

static const short _chunked_cond_keys[] = {
	0u, 255u, 0u, 12u, 13u, 13u, 14u, 255u, 
	0
};

static const char _chunked_cond_spaces[] = {
	0, 0, 0, 0, 0
};

static const char _chunked_key_offsets[] = {
	0, 0, 11, 20, 21, 22, 23, 25, 
	34, 37, 38, 40, 43, 44, 51
};

static const short _chunked_trans_keys[] = {
	13u, 32u, 48u, 9u, 10u, 49u, 57u, 65u, 
	70u, 97u, 102u, 13u, 32u, 48u, 49u, 57u, 
	65u, 70u, 97u, 102u, 10u, 13u, 10u, 13u, 
	32u, 9u, 13u, 32u, 48u, 57u, 65u, 70u, 
	97u, 102u, 9u, 13u, 32u, 10u, 512u, 767u, 
	269u, 512u, 767u, 10u, 48u, 49u, 57u, 65u, 
	70u, 97u, 102u, 0
};

static const char _chunked_single_lengths[] = {
	0, 3, 3, 1, 1, 1, 2, 3, 
	3, 1, 0, 1, 1, 1, 0
};

static const char _chunked_range_lengths[] = {
	0, 4, 3, 0, 0, 0, 0, 3, 
	0, 0, 1, 1, 0, 3, 0
};

static const char _chunked_index_offsets[] = {
	0, 0, 8, 15, 17, 19, 21, 24, 
	31, 35, 37, 39, 42, 44, 49
};

static const char _chunked_trans_targs[] = {
	1, 1, 2, 1, 7, 7, 7, 0, 
	3, 6, 2, 7, 7, 7, 0, 4, 
	0, 5, 0, 14, 0, 3, 6, 0, 
	8, 9, 8, 7, 7, 7, 0, 8, 
	9, 8, 0, 10, 0, 11, 0, 12, 
	11, 0, 13, 0, 2, 7, 7, 7, 
	0, 0, 0
};

static const char _chunked_trans_actions[] = {
	0, 0, 1, 0, 1, 1, 1, 0, 
	0, 0, 3, 3, 3, 3, 2, 0, 
	2, 0, 4, 6, 5, 0, 0, 2, 
	0, 0, 0, 3, 3, 3, 7, 0, 
	0, 0, 7, 0, 7, 9, 8, 0, 
	11, 10, 13, 12, 1, 1, 1, 1, 
	12, 5, 0
};

static const char _chunked_eof_actions[] = {
	0, 0, 2, 2, 4, 5, 2, 7, 
	7, 7, 8, 10, 12, 12, 0
};

static const int chunked_start = 1;
static const int chunked_first_final = 14;
static const int chunked_error = 0;

static const int chunked_en_main = 1;


#line 18 "src/chunked_parser.rl"


ngx_int_t
ngx_http_chunkin_init_chunked_parser(ngx_http_request_t *r,
        ngx_http_chunkin_ctx_t *ctx)
{
    int cs;

    
#line 114 "src/chunked_parser.c"
	{
	cs = chunked_start;
	}

#line 27 "src/chunked_parser.rl"

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

    
#line 182 "src/chunked_parser.rl"


    
#line 157 "src/chunked_parser.c"
	{
	int _klen;
	const short *_keys;
	int _trans;
	short _widec;

	if ( p == pe )
		goto _test_eof;
	if ( cs == 0 )
		goto _out;
_resume:
	_widec = (*p);
	_klen = _chunked_cond_lengths[cs];
	_keys = _chunked_cond_keys + (_chunked_cond_offsets[cs]*2);
	if ( _klen > 0 ) {
		const short *_lower = _keys;
		const short *_mid;
		const short *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( _widec < _mid[0] )
				_upper = _mid - 2;
			else if ( _widec > _mid[1] )
				_lower = _mid + 2;
			else {
				switch ( _chunked_cond_spaces[_chunked_cond_offsets[cs] + ((_mid - _keys)>>1)] ) {
	case 0: {
		_widec = (short)(256u + ((*p) - 0u));
		if ( 
#line 66 "src/chunked_parser.rl"

            ctx->chunk_bytes_read < ctx->chunk_size
         ) _widec += 256;
		break;
	}
				}
				break;
			}
		}
	}

	_keys = _chunked_trans_keys + _chunked_key_offsets[cs];
	_trans = _chunked_index_offsets[cs];

	_klen = _chunked_single_lengths[cs];
	if ( _klen > 0 ) {
		const short *_lower = _keys;
		const short *_mid;
		const short *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( _widec < *_mid )
				_upper = _mid - 1;
			else if ( _widec > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _chunked_range_lengths[cs];
	if ( _klen > 0 ) {
		const short *_lower = _keys;
		const short *_mid;
		const short *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( _widec < _mid[0] )
				_upper = _mid - 2;
			else if ( _widec > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += ((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	cs = _chunked_trans_targs[_trans];

	if ( _chunked_trans_actions[_trans] == 0 )
		goto _again;

	switch ( _chunked_trans_actions[_trans] ) {
	case 6:
#line 62 "src/chunked_parser.rl"
	{
            done = 1;
        }
	break;
	case 11:
#line 70 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	break;
	case 3:
#line 89 "src/chunked_parser.rl"
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
	break;
	case 13:
#line 123 "src/chunked_parser.rl"
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
	break;
	case 10:
#line 158 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	break;
	case 12:
#line 162 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data_terminator"; }
	break;
	case 1:
#line 83 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
        }
#line 89 "src/chunked_parser.rl"
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
	break;
	case 9:
#line 104 "src/chunked_parser.rl"
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

            b->last = b->pos = p;
            b->memory = 1;
        }
#line 70 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read++;

            ctx->chunk->buf->last = p + 1;

            ctx->chunks_total_size++;

            dd("bytes read: %d (char '%c', bytes read %d, chunk size %d)", ctx->chunk->buf->last - ctx->chunk->buf->pos, *p, ctx->chunk_bytes_read, ctx->chunk_size);
            ngx_log_debug2(NGX_LOG_DEBUG_HTTP, c->log, 0,
                    "chunkin: data bytes read: %uz (char: \"%c\")\n",
                    ctx->chunk_bytes_read, *p);
        }
	break;
	case 7:
#line 149 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 170 "src/chunked_parser.rl"
	{ err_ctx = "chunk_size"; }
	break;
	case 2:
#line 149 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 174 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
	break;
	case 5:
#line 149 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 177 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	break;
	case 8:
#line 149 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 170 "src/chunked_parser.rl"
	{ err_ctx = "chunk_size"; }
#line 158 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	break;
	case 4:
#line 149 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 174 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
#line 177 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	break;
#line 422 "src/chunked_parser.c"
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	if ( p == eof )
	{
	switch ( _chunked_eof_actions[cs] ) {
	case 10:
#line 158 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	break;
	case 12:
#line 162 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data_terminator"; }
	break;
	case 7:
#line 149 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 170 "src/chunked_parser.rl"
	{ err_ctx = "chunk_size"; }
	break;
	case 2:
#line 149 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 174 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
	break;
	case 5:
#line 149 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 177 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	break;
	case 8:
#line 149 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 170 "src/chunked_parser.rl"
	{ err_ctx = "chunk_size"; }
#line 158 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	break;
	case 4:
#line 149 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 174 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
#line 177 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	break;
#line 476 "src/chunked_parser.c"
	}
	}

	_out: {}
	}

#line 185 "src/chunked_parser.rl"

    ctx->parser_state = cs;

    *pos_addr = p;

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

#if EXTENDED_DEBUG

        headers_buf.data = r->header_in->start;
        headers_buf.len = ctx->saved_header_in_pos - r->header_in->start;

        if (strcmp(caller_info, "preread") == 0) {
            preread_buf.data = pos;
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

