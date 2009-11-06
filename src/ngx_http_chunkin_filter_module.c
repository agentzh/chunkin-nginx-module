#define DDEBUG 1
#include "ddebug.h"

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>

#include "ngx_http_chunkin_request_body.h"

static ngx_str_t ngx_http_chunkin_content_length_header_key =
    ngx_string("Content-Length");

typedef struct {
    ngx_flag_t          enabled;
} ngx_http_chunkin_conf_t;

typedef struct {
    ngx_flag_t          ignore_body;
} ngx_http_chunkin_ctx_t;

static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;

static ngx_http_output_body_filter_pt    ngx_http_next_body_filter;

static ngx_int_t ngx_http_chunkin_header_filter(ngx_http_request_t *r);
static ngx_int_t ngx_http_chunkin_body_filter(ngx_http_request_t *r,
        ngx_chain_t *in);

static void *ngx_http_chunkin_create_conf(ngx_conf_t *cf);
static char *ngx_http_chunkin_merge_conf(ngx_conf_t *cf, void *parent,
    void *child);
static ngx_int_t ngx_http_chunkin_filter_init(ngx_conf_t *cf);

static ngx_command_t  ngx_http_chunkin_commands[] = {

    { ngx_string("chunkin"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_chunkin_conf_t, enabled),
      NULL },

      ngx_null_command
};

static ngx_http_module_t  ngx_http_chunkin_filter_module_ctx = {
    NULL,                                  /* preconfiguration */
    ngx_http_chunkin_filter_init,         /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    ngx_http_chunkin_create_conf,         /* create location configuration */
    ngx_http_chunkin_merge_conf           /* merge location configuration */
};


ngx_module_t  ngx_http_chunkin_filter_module = {
    NGX_MODULE_V1,
    &ngx_http_chunkin_filter_module_ctx,  /* module context */
    ngx_http_chunkin_commands,            /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};

static void *
ngx_http_chunkin_create_conf(ngx_conf_t *cf)
{
    ngx_http_chunkin_conf_t  *conf;

    conf = ngx_palloc(cf->pool, sizeof(ngx_http_chunkin_conf_t));
    if (conf == NULL) {
        return NULL;
    }

    conf->enabled = NGX_CONF_UNSET;

    return conf;
}

static char *
ngx_http_chunkin_merge_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_chunkin_conf_t *prev = parent;
    ngx_http_chunkin_conf_t *conf = child;

    ngx_conf_merge_value(conf->enabled, prev->enabled, 0);

    return NGX_CONF_OK;
}

static ngx_int_t
ngx_http_chunkin_filter_init(ngx_conf_t *cf)
{
    ngx_http_next_header_filter = ngx_http_top_header_filter;
    ngx_http_top_header_filter = ngx_http_chunkin_header_filter;

    ngx_http_next_body_filter = ngx_http_top_body_filter;
    ngx_http_top_body_filter = ngx_http_chunkin_body_filter;

    return NGX_OK;
}

static ngx_flag_t
ngx_http_chunkin_is_chunked_encoding(ngx_http_request_t *r) {
    return r->headers_in.transfer_encoding &&
        r->headers_in.transfer_encoding->value.len >= 7 &&
        ngx_strcasestrn(r->headers_in.transfer_encoding->value.data,
                "chunked", 7 - 1);
}

static ngx_int_t
ngx_http_chunkin_header_filter(ngx_http_request_t *r)
{
    ngx_http_chunkin_ctx_t      *ctx;
    ngx_http_chunkin_conf_t     *conf;

    conf = ngx_http_get_module_loc_conf(r, ngx_http_chunkin_filter_module);

    DD("chunkin enabled? %d", conf->enabled);
    DD("length required status? %d",
            r->headers_out.status == NGX_HTTP_LENGTH_REQUIRED);
    DD("is chunked? %d",
            ngx_http_chunkin_is_chunked_encoding(r));

    if (r != r->main) {
        DD("We ignore subrequests.");
        return ngx_http_next_header_filter(r);
    }

    if ( ! conf->enabled || r->headers_out.status != NGX_HTTP_LENGTH_REQUIRED
            || ! ngx_http_chunkin_is_chunked_encoding(r))
    {
        DD("inactive");
        return ngx_http_next_header_filter(r);
    }

    ctx = ngx_http_get_module_ctx(r, ngx_http_chunkin_filter_module);

    if (ctx) {
        DD("HAHA Found ctx...skipped.");
        return ngx_http_next_header_filter(r);
    }

    ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_chunkin_ctx_t));
    if (ctx == NULL) {
        return NGX_ERROR;
    }

    ctx->ignore_body = 1;

    ngx_http_set_ctx(r, ctx, ngx_http_chunkin_filter_module);

    return NGX_OK;
}

static ngx_int_t
ngx_http_chunkin_body_filter(ngx_http_request_t *r, ngx_chain_t *in)
{
    ngx_http_chunkin_ctx_t      *ctx;
    ngx_flag_t                  last;
    ngx_int_t                   rc;
    ngx_chain_t                 *cl;
    ngx_table_elt_t             *h;

    ctx = ngx_http_get_module_ctx(r, ngx_http_chunkin_filter_module);

    if (ctx == NULL || !ctx->ignore_body) {
        DD("ctx NULL? %s", ctx == NULL ? "yes" : "NO");
        return ngx_http_next_body_filter(r, in);
    }

    /* throw away the 411 response body */

    last = 0;

    for (cl = in; cl; cl = cl->next) {
        if (cl->buf->last_buf) {
            last = 1;
        }
    }

    if (last) {
        DD("ignore last body...");

        r->uri_changes++;

        r->err_status = 0;
        r->error_page = 0;

        r->headers_in.transfer_encoding->value.len = 0;
        r->headers_in.transfer_encoding->value.data = (u_char*) "";

        /* XXX this is a hack */
        r->headers_in.content_length_n = 0;

        r->headers_in.content_length = ngx_pcalloc(r->pool,
                sizeof(ngx_table_elt_t));
        if (r->headers_in.content_length == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        r->headers_in.content_length->value.data =
            ngx_palloc(r->pool, NGX_OFF_T_LEN);
        if (r->headers_in.content_length->value.data == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }
        r->headers_in.content_length->value.len = ngx_sprintf(
                r->headers_in.content_length->value.data, "%O",
                r->headers_in.content_length_n) -
                r->headers_in.content_length->value.data;

        h = ngx_list_push(&r->headers_in.headers);
        if (h == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        h->hash = r->header_hash;

        h->key = ngx_http_chunkin_content_length_header_key;
        h->value = r->headers_in.content_length->value;

        h->lowcase_key = ngx_pnalloc(r->pool, h->key.len);
        if (h->lowcase_key == NULL) {
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        }

        ngx_strlow(h->lowcase_key, h->key.data, h->key.len);

        rc = ngx_http_internal_redirect(r, &r->uri, &r->args);
        return rc;
    }

    DD("ignore body...");

    return NGX_OK;
}

