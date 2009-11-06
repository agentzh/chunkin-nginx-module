#define DDEBUG 0
#include "ddebug.h"

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>
#include <nginx.h>

#include "ngx_http_chunkin_request_body.h"

typedef struct {
    ngx_flag_t          enabled;
} ngx_http_chunkin_conf_t;

typedef struct {
} ngx_http_chunkin_ctx_t;

static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;

static void *ngx_http_chunkin_create_conf(ngx_conf_t *cf);
static char *ngx_http_chunkin_merge_conf(ngx_conf_t *cf, void *parent,
    void *child);
static ngx_int_t ngx_http_chunkin_filter_init(ngx_conf_t *cf);

static ngx_command_t  ngx_http_chunkin_commands[] = {

    { ngx_string("chunkin"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
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

static ngx_flag_t
ngx_http_chunkin_is_chunked_encoding(ngx_http_request_t *r) {
    return r->headers_in.transfer_encoding &&
        ngx_strcasestrn(r->headers_in.transfer_encoding->value.data,
                "chunked", 7 - 1);
}

static ngx_int_t
ngx_http_chunkin_header_filter(ngx_http_request_t *r)
{
    ngx_http_chunkin_ctx_t   *ctx;
    ngx_http_chunkin_conf_t  *conf;

    conf = ngx_http_get_module_loc_conf(r, ngx_http_chunkin_filter_module);

    if ( ! conf->enabled || r->headers_out.stats != NGX_HTTP_LENGTH_REQUIRED
            || ngx_http_chunkin_is_chunked_encoding(r))
    {
        return ngx_http_next_header_filter(r);
    }

    ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_chunkin_ctx_t));
    if (ctx == NULL) {
        return NGX_ERROR;
    }

    ngx_http_set_ctx(r, ctx, ngx_http_chunkin_filter_module);

    return ngx_http_next_header_filter(r);
}

