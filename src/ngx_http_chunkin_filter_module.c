/* Copyright (C) agentzh */

#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"

#include <ngx_config.h>
#include <nginx.h>

#include "ngx_http_chunkin_util.h"
#include "ngx_http_chunkin_filter_module.h"
#include "ngx_http_chunkin_request_body.h"

enum {
    DEFAULT_MAX_CHUNKS_PER_BUF = 512
};

static void *ngx_http_chunkin_create_conf(ngx_conf_t *cf);
static char *ngx_http_chunkin_merge_conf(ngx_conf_t *cf, void *parent,
    void *child);


static ngx_command_t  ngx_http_chunkin_commands[] = {

    { ngx_string("chunkin_max_chunks_per_buf"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_num_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_chunkin_conf_t, max_chunks_per_buf),
      NULL },

    { ngx_string("chunkin_keepalive"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF
          |NGX_HTTP_LIF_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_chunkin_conf_t, keepalive),
      NULL },

      ngx_null_command
};


static ngx_http_module_t  ngx_http_chunkin_filter_module_ctx = {
    NULL,                                 /* preconfiguration */
    NULL,                                 /* postconfiguration */

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
    conf->keepalive = NGX_CONF_UNSET;
    conf->max_chunks_per_buf = NGX_CONF_UNSET_UINT;

    return conf;
}


static char *
ngx_http_chunkin_merge_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_chunkin_conf_t *prev = parent;
    ngx_http_chunkin_conf_t *conf = child;

    ngx_conf_merge_value(conf->keepalive, prev->keepalive, 0);
    ngx_conf_merge_uint_value(conf->max_chunks_per_buf,
            prev->max_chunks_per_buf,
            DEFAULT_MAX_CHUNKS_PER_BUF);

    return NGX_CONF_OK;
}

