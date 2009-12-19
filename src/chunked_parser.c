
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
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 4, 4, 4, 4, 4, 4, 
	4, 4, 4, 4, 4, 4, 4, 4, 
	4, 4, 4, 4
};

static const char _chunked_cond_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	1, 3, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0
};

static const short _chunked_cond_keys[] = {
	0u, 255u, 0u, 12u, 13u, 13u, 14u, 255u, 
	0
};

static const char _chunked_cond_spaces[] = {
	0, 0, 0, 0, 0
};

static const short _chunked_key_offsets[] = {
	0, 0, 7, 18, 22, 23, 24, 25, 
	41, 60, 65, 81, 99, 106, 109, 116, 
	125, 142, 162, 172, 189, 208, 218, 222, 
	223, 225, 228, 229, 236, 252, 271, 276, 
	292, 310, 314, 321, 324, 331, 340, 357, 
	377, 387, 404, 423
};

static const short _chunked_trans_keys[] = {
	48u, 49u, 57u, 65u, 70u, 97u, 102u, 9u, 
	13u, 32u, 48u, 59u, 49u, 57u, 65u, 70u, 
	97u, 102u, 9u, 13u, 32u, 59u, 10u, 13u, 
	10u, 9u, 32u, 34u, 44u, 47u, 123u, 125u, 
	127u, 0u, 31u, 40u, 41u, 58u, 64u, 91u, 
	93u, 9u, 13u, 32u, 34u, 44u, 47u, 59u, 
	61u, 123u, 125u, 127u, 0u, 31u, 40u, 41u, 
	58u, 64u, 91u, 93u, 9u, 13u, 32u, 59u, 
	61u, 9u, 32u, 34u, 44u, 47u, 123u, 125u, 
	127u, 0u, 31u, 40u, 41u, 58u, 64u, 91u, 
	93u, 9u, 13u, 32u, 34u, 44u, 47u, 59u, 
	123u, 125u, 127u, 0u, 31u, 40u, 41u, 58u, 
	64u, 91u, 93u, 34u, 92u, 127u, 0u, 8u, 
	10u, 31u, 13u, 34u, 92u, 34u, 92u, 127u, 
	0u, 8u, 10u, 31u, 9u, 13u, 32u, 34u, 
	59u, 92u, 127u, 0u, 31u, 9u, 32u, 34u, 
	44u, 47u, 92u, 123u, 125u, 127u, 0u, 31u, 
	40u, 41u, 58u, 64u, 91u, 93u, 9u, 13u, 
	32u, 34u, 44u, 47u, 59u, 61u, 92u, 123u, 
	125u, 127u, 0u, 31u, 40u, 41u, 58u, 64u, 
	91u, 93u, 9u, 13u, 32u, 34u, 59u, 61u, 
	92u, 127u, 0u, 31u, 9u, 32u, 34u, 44u, 
	47u, 92u, 123u, 125u, 127u, 0u, 31u, 40u, 
	41u, 58u, 64u, 91u, 93u, 9u, 13u, 32u, 
	34u, 44u, 47u, 59u, 92u, 123u, 125u, 127u, 
	0u, 31u, 40u, 41u, 58u, 64u, 91u, 93u, 
	9u, 13u, 32u, 59u, 48u, 57u, 65u, 70u, 
	97u, 102u, 9u, 13u, 32u, 59u, 10u, 512u, 
	767u, 269u, 512u, 767u, 10u, 48u, 49u, 57u, 
	65u, 70u, 97u, 102u, 9u, 32u, 34u, 44u, 
	47u, 123u, 125u, 127u, 0u, 31u, 40u, 41u, 
	58u, 64u, 91u, 93u, 9u, 13u, 32u, 34u, 
	44u, 47u, 59u, 61u, 123u, 125u, 127u, 0u, 
	31u, 40u, 41u, 58u, 64u, 91u, 93u, 9u, 
	13u, 32u, 59u, 61u, 9u, 32u, 34u, 44u, 
	47u, 123u, 125u, 127u, 0u, 31u, 40u, 41u, 
	58u, 64u, 91u, 93u, 9u, 13u, 32u, 34u, 
	44u, 47u, 59u, 123u, 125u, 127u, 0u, 31u, 
	40u, 41u, 58u, 64u, 91u, 93u, 9u, 13u, 
	32u, 59u, 34u, 92u, 127u, 0u, 8u, 10u, 
	31u, 13u, 34u, 92u, 34u, 92u, 127u, 0u, 
	8u, 10u, 31u, 9u, 13u, 32u, 34u, 59u, 
	92u, 127u, 0u, 31u, 9u, 32u, 34u, 44u, 
	47u, 92u, 123u, 125u, 127u, 0u, 31u, 40u, 
	41u, 58u, 64u, 91u, 93u, 9u, 13u, 32u, 
	34u, 44u, 47u, 59u, 61u, 92u, 123u, 125u, 
	127u, 0u, 31u, 40u, 41u, 58u, 64u, 91u, 
	93u, 9u, 13u, 32u, 34u, 59u, 61u, 92u, 
	127u, 0u, 31u, 9u, 32u, 34u, 44u, 47u, 
	92u, 123u, 125u, 127u, 0u, 31u, 40u, 41u, 
	58u, 64u, 91u, 93u, 9u, 13u, 32u, 34u, 
	44u, 47u, 59u, 92u, 123u, 125u, 127u, 0u, 
	31u, 40u, 41u, 58u, 64u, 91u, 93u, 0
};

static const char _chunked_single_lengths[] = {
	0, 1, 5, 4, 1, 1, 1, 8, 
	11, 5, 8, 10, 3, 3, 3, 7, 
	9, 12, 8, 9, 11, 4, 4, 1, 
	0, 1, 1, 1, 8, 11, 5, 8, 
	10, 4, 3, 3, 3, 7, 9, 12, 
	8, 9, 11, 0
};

static const char _chunked_range_lengths[] = {
	0, 3, 3, 0, 0, 0, 0, 4, 
	4, 0, 4, 4, 2, 0, 2, 1, 
	4, 4, 1, 4, 4, 3, 0, 0, 
	1, 1, 0, 3, 4, 4, 0, 4, 
	4, 0, 2, 0, 2, 1, 4, 4, 
	1, 4, 4, 0
};

static const short _chunked_index_offsets[] = {
	0, 0, 5, 14, 19, 21, 23, 25, 
	38, 54, 60, 73, 88, 94, 98, 104, 
	113, 127, 144, 154, 168, 184, 192, 197, 
	199, 201, 204, 206, 211, 224, 240, 246, 
	259, 274, 279, 285, 289, 295, 304, 318, 
	335, 345, 359, 375
};

static const char _chunked_indicies[] = {
	0, 2, 2, 2, 1, 4, 5, 4, 
	6, 8, 7, 7, 7, 3, 4, 5, 
	4, 8, 3, 10, 9, 12, 11, 14, 
	13, 8, 8, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 15, 16, 5, 
	16, 3, 3, 3, 8, 17, 3, 3, 
	3, 3, 3, 3, 3, 15, 16, 5, 
	16, 8, 17, 3, 17, 17, 19, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	18, 4, 5, 4, 3, 3, 3, 8, 
	3, 3, 3, 3, 3, 3, 3, 18, 
	4, 20, 1, 1, 1, 19, 21, 22, 
	20, 19, 4, 20, 23, 23, 23, 19, 
	22, 5, 22, 4, 24, 20, 3, 3, 
	19, 24, 24, 4, 19, 19, 20, 19, 
	19, 1, 1, 19, 19, 19, 25, 26, 
	5, 26, 4, 19, 19, 24, 27, 20, 
	19, 19, 3, 3, 19, 19, 19, 25, 
	26, 5, 26, 4, 24, 27, 20, 3, 
	3, 19, 27, 27, 22, 19, 19, 20, 
	19, 19, 1, 1, 19, 19, 19, 28, 
	22, 5, 22, 4, 19, 19, 24, 20, 
	19, 19, 3, 3, 19, 19, 19, 28, 
	30, 31, 30, 32, 7, 7, 7, 29, 
	30, 31, 30, 32, 29, 34, 33, 36, 
	35, 38, 39, 37, 41, 40, 0, 2, 
	2, 2, 40, 32, 32, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 42, 
	44, 31, 44, 43, 43, 43, 32, 45, 
	43, 43, 43, 43, 43, 43, 43, 42, 
	44, 31, 44, 32, 45, 43, 45, 45, 
	47, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 46, 48, 31, 48, 43, 43, 
	43, 32, 43, 43, 43, 43, 43, 43, 
	43, 46, 48, 31, 48, 32, 43, 48, 
	49, 1, 1, 1, 47, 50, 51, 49, 
	47, 48, 49, 23, 23, 23, 47, 51, 
	31, 51, 48, 52, 49, 43, 43, 47, 
	52, 52, 48, 47, 47, 49, 47, 47, 
	1, 1, 47, 47, 47, 53, 54, 31, 
	54, 48, 47, 47, 52, 55, 49, 47, 
	47, 43, 43, 47, 47, 47, 53, 54, 
	31, 54, 48, 52, 55, 49, 43, 43, 
	47, 55, 55, 51, 47, 47, 49, 47, 
	47, 1, 1, 47, 47, 47, 56, 51, 
	31, 51, 48, 47, 47, 52, 49, 47, 
	47, 43, 43, 47, 47, 47, 56, 13, 
	0
};

static const char _chunked_trans_targs[] = {
	2, 0, 21, 0, 3, 4, 2, 21, 
	7, 0, 5, 0, 6, 0, 43, 8, 
	9, 10, 11, 12, 13, 14, 15, 0, 
	16, 17, 18, 19, 20, 0, 22, 23, 
	28, 0, 24, 0, 25, 0, 26, 25, 
	0, 27, 29, 0, 30, 31, 32, 34, 
	33, 35, 36, 37, 38, 39, 40, 41, 
	42
};

static const char _chunked_trans_actions[] = {
	1, 0, 1, 2, 0, 0, 3, 3, 
	0, 4, 0, 5, 0, 6, 7, 0, 
	0, 0, 0, 0, 0, 0, 0, 8, 
	0, 0, 0, 0, 0, 9, 0, 0, 
	0, 10, 0, 11, 12, 13, 0, 14, 
	15, 16, 0, 17, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0
};

static const char _chunked_eof_actions[] = {
	0, 0, 2, 2, 4, 5, 6, 0, 
	2, 2, 0, 2, 0, 0, 8, 2, 
	0, 2, 2, 0, 2, 9, 9, 10, 
	11, 13, 15, 15, 0, 17, 17, 0, 
	17, 17, 0, 0, 8, 17, 0, 17, 
	17, 0, 17, 0
};

static const int chunked_start = 1;
static const int chunked_first_final = 43;
static const int chunked_error = 0;

static const int chunked_en_main = 1;


#line 18 "src/chunked_parser.rl"


ngx_int_t
ngx_http_chunkin_init_chunked_parser(ngx_http_request_t *r,
        ngx_http_chunkin_ctx_t *ctx)
{
    int cs;

    
#line 241 "src/chunked_parser.c"
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

    
#line 221 "src/chunked_parser.rl"


    
#line 284 "src/chunked_parser.c"
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
	_trans = _chunked_indicies[_trans];
	cs = _chunked_trans_targs[_trans];

	if ( _chunked_trans_actions[_trans] == 0 )
		goto _again;

	switch ( _chunked_trans_actions[_trans] ) {
	case 7:
#line 62 "src/chunked_parser.rl"
	{
            done = 1;
        }
	break;
	case 14:
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
	case 16:
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
	case 8:
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
	break;
	case 13:
#line 162 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	break;
	case 15:
#line 166 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data_terminator"; }
	break;
	case 17:
#line 206 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
	break;
	case 2:
#line 212 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
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
	case 12:
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
	case 10:
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 206 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
	break;
	case 4:
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 212 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
	break;
	case 6:
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 216 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	break;
	case 9:
#line 203 "src/chunked_parser.rl"
	{ err_ctx = "chunk_size"; }
#line 206 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
	break;
	case 11:
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 206 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
#line 162 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	break;
	case 5:
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 212 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
#line 216 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	break;
#line 568 "src/chunked_parser.c"
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
	case 8:
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
	break;
	case 13:
#line 162 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	break;
	case 15:
#line 166 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data_terminator"; }
	break;
	case 17:
#line 206 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
	break;
	case 2:
#line 212 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
	break;
	case 10:
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 206 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
	break;
	case 4:
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 212 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
	break;
	case 6:
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 216 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	break;
	case 9:
#line 203 "src/chunked_parser.rl"
	{ err_ctx = "chunk_size"; }
#line 206 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
	break;
	case 11:
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 206 "src/chunked_parser.rl"
	{ err_ctx = "chunk_ext"; }
#line 162 "src/chunked_parser.rl"
	{ err_ctx = "chunk_data"; }
	break;
	case 5:
#line 153 "src/chunked_parser.rl"
	{ err_ctx = "CRLF"; }
#line 212 "src/chunked_parser.rl"
	{ err_ctx = "last_chunk"; }
#line 216 "src/chunked_parser.rl"
	{ err_ctx = "parser"; }
	break;
#line 640 "src/chunked_parser.c"
	}
	}

	_out: {}
	}

#line 224 "src/chunked_parser.rl"

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

