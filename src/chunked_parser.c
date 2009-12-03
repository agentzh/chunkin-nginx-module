#line 1 "src/chunked_parser.rl"
#define DDEBUG 1

#include "ddebug.h"

#include "chunked_parser.h"
#include "ngx_http_chunkin_util.h"

enum {
    PRE_TEXT_LEN = 15,
    POST_TEXT_LEN = 15
};

#line 14 "src/chunked_parser.rl"

#line 17 "src/chunked_parser.c"
static const char _chunked_actions[] = {
	0, 1, 0, 1, 1, 1, 3, 1, 
	4, 1, 5, 2, 1, 5, 2, 2, 
	3, 2, 3, 1, 2, 4, 1, 2, 
	5, 0, 3, 1, 2, 3, 3, 1, 
	5, 0, 4, 4, 1, 2, 3
};

static const char _chunked_cond_offsets[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 1, 1, 2, 3, 4, 5, 
	5, 5, 6, 7, 7, 8, 9, 9, 
	10, 10, 11, 12, 13, 13, 13
};

static const char _chunked_cond_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 1, 0, 1, 1, 1, 1, 0, 
	0, 1, 1, 0, 1, 1, 0, 1, 
	0, 1, 1, 1, 0, 0, 1
};

static const short _chunked_cond_keys[] = {
	0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u, 
	0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u, 
	0u, 255u, 0u, 255u, 0u, 255u, 0u, 255u, 
	0u, 255u, 0u, 255u, 0
};

static const char _chunked_cond_spaces[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0
};

static const unsigned char _chunked_key_offsets[] = {
	0, 0, 7, 15, 16, 17, 18, 20, 
	28, 29, 33, 34, 38, 44, 68, 92, 
	93, 101, 107, 131, 132, 138, 162, 163, 
	187, 189, 195, 201, 207, 207, 214
};

static const short _chunked_trans_keys[] = {
	48u, 49u, 57u, 65u, 70u, 97u, 102u, 13u, 
	32u, 48u, 57u, 65u, 70u, 97u, 102u, 10u, 
	13u, 10u, 13u, 32u, 13u, 32u, 48u, 57u, 
	65u, 70u, 97u, 102u, 10u, 269u, 525u, 512u, 
	767u, 10u, 269u, 525u, 512u, 767u, 266u, 269u, 
	522u, 525u, 512u, 767u, 269u, 304u, 525u, 560u, 
	305u, 313u, 321u, 326u, 353u, 358u, 512u, 559u, 
	561u, 569u, 570u, 576u, 577u, 582u, 583u, 608u, 
	609u, 614u, 615u, 767u, 269u, 288u, 525u, 544u, 
	304u, 313u, 321u, 326u, 353u, 358u, 512u, 559u, 
	560u, 569u, 570u, 576u, 577u, 582u, 583u, 608u, 
	609u, 614u, 615u, 767u, 10u, 13u, 48u, 49u, 
	57u, 65u, 70u, 97u, 102u, 266u, 269u, 522u, 
	525u, 512u, 767u, 269u, 304u, 525u, 560u, 305u, 
	313u, 321u, 326u, 353u, 358u, 512u, 559u, 561u, 
	569u, 570u, 576u, 577u, 582u, 583u, 608u, 609u, 
	614u, 615u, 767u, 10u, 266u, 269u, 522u, 525u, 
	512u, 767u, 269u, 288u, 525u, 544u, 304u, 313u, 
	321u, 326u, 353u, 358u, 512u, 559u, 560u, 569u, 
	570u, 576u, 577u, 582u, 583u, 608u, 609u, 614u, 
	615u, 767u, 10u, 269u, 304u, 525u, 560u, 305u, 
	313u, 321u, 326u, 353u, 358u, 512u, 559u, 561u, 
	569u, 570u, 576u, 577u, 582u, 583u, 608u, 609u, 
	614u, 615u, 767u, 13u, 32u, 266u, 269u, 522u, 
	525u, 512u, 767u, 269u, 288u, 525u, 544u, 512u, 
	767u, 269u, 288u, 525u, 544u, 512u, 767u, 48u, 
	49u, 57u, 65u, 70u, 97u, 102u, 269u, 304u, 
	525u, 560u, 305u, 313u, 321u, 326u, 353u, 358u, 
	512u, 559u, 561u, 569u, 570u, 576u, 577u, 582u, 
	583u, 608u, 609u, 614u, 615u, 767u, 0
};

static const char _chunked_single_lengths[] = {
	0, 1, 2, 1, 1, 1, 2, 2, 
	1, 2, 1, 2, 4, 4, 4, 1, 
	2, 4, 4, 1, 4, 4, 1, 4, 
	2, 4, 4, 4, 0, 1, 4
};

static const char _chunked_range_lengths[] = {
	0, 3, 3, 0, 0, 0, 0, 3, 
	0, 1, 0, 1, 1, 10, 10, 0, 
	3, 1, 10, 0, 1, 10, 0, 10, 
	0, 1, 1, 1, 0, 3, 10
};

static const unsigned char _chunked_index_offsets[] = {
	0, 0, 5, 11, 13, 15, 17, 20, 
	26, 28, 32, 34, 38, 44, 59, 74, 
	76, 82, 88, 103, 105, 111, 126, 128, 
	143, 146, 152, 158, 164, 165, 170
};

static const char _chunked_indicies[] = {
	0, 2, 2, 2, 1, 3, 4, 5, 
	5, 5, 1, 6, 1, 7, 1, 8, 
	1, 3, 4, 1, 9, 10, 5, 5, 
	5, 1, 11, 1, 12, 14, 13, 1, 
	15, 1, 16, 18, 17, 1, 15, 16, 
	19, 18, 17, 1, 16, 0, 18, 20, 
	2, 2, 2, 17, 21, 17, 21, 17, 
	21, 17, 1, 22, 4, 23, 24, 5, 
	5, 5, 17, 25, 17, 25, 17, 25, 
	17, 1, 26, 1, 7, 0, 2, 2, 
	2, 1, 26, 16, 27, 18, 17, 1, 
	28, 0, 29, 20, 2, 2, 2, 17, 
	21, 17, 21, 17, 21, 17, 1, 30, 
	1, 30, 16, 31, 18, 17, 1, 32, 
	10, 33, 34, 5, 5, 5, 17, 25, 
	17, 25, 17, 25, 17, 1, 35, 1, 
	12, 0, 14, 36, 2, 2, 2, 13, 
	37, 13, 37, 13, 37, 13, 1, 9, 
	10, 1, 35, 16, 38, 18, 17, 1, 
	32, 10, 33, 34, 17, 1, 22, 4, 
	23, 24, 17, 1, 1, 0, 2, 2, 
	2, 1, 16, 0, 18, 20, 2, 2, 
	2, 17, 21, 17, 21, 17, 21, 17, 
	1, 0
};

static const char _chunked_trans_targs[] = {
	2, 0, 7, 3, 6, 7, 4, 5, 
	28, 8, 24, 9, 10, 11, 12, 1, 
	10, 11, 12, 13, 14, 21, 15, 17, 
	27, 21, 16, 18, 19, 20, 29, 30, 
	22, 25, 26, 23, 14, 21, 23
};

static const char _chunked_trans_actions[] = {
	14, 0, 14, 0, 0, 5, 0, 0, 
	1, 0, 0, 0, 7, 20, 20, 9, 
	0, 3, 3, 11, 26, 26, 0, 3, 
	3, 17, 9, 11, 0, 3, 23, 30, 
	0, 3, 3, 9, 34, 34, 11
};

static const int chunked_start = 1;
static const int chunked_first_final = 28;
static const int chunked_error = 0;

static const int chunked_en_main = 1;

#line 15 "src/chunked_parser.rl"

ngx_int_t
ngx_http_chunkin_init_chunked_parser(ngx_http_request_t *r,
        ngx_http_chunkin_ctx_t *ctx)
{
    int cs;

    
#line 171 "src/chunked_parser.c"
	{
	cs = chunked_start;
	}
#line 23 "src/chunked_parser.rl"

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

    #line 161 "src/chunked_parser.rl"


    
#line 208 "src/chunked_parser.c"
	{
	int _klen;
	unsigned int _trans;
	short _widec;
	const char *_acts;
	unsigned int _nacts;
	const short *_keys;

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
#line 58 "src/chunked_parser.rl"

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

	_acts = _chunked_actions + _chunked_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 54 "src/chunked_parser.rl"
	{
            done = 1;
        }
	break;
	case 1:
#line 62 "src/chunked_parser.rl"
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
	case 2:
#line 75 "src/chunked_parser.rl"
	{
            ctx->chunk_bytes_read = 0;
            ctx->chunk_size = 0;
            ctx->chunk_size_order = 0;
        }
	break;
	case 3:
#line 81 "src/chunked_parser.rl"
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
	case 4:
#line 96 "src/chunked_parser.rl"
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
	break;
	case 5:
#line 113 "src/chunked_parser.rl"
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
#line 409 "src/chunked_parser.c"
		}
	}

_again:
	if ( cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}
#line 164 "src/chunked_parser.rl"

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

