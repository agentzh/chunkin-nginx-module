/* Copyright (C) agentzh */

#define DDEBUG 0
#include "ddebug.h"

#include <ngx_config.h>

#include "ngx_http_chunkin_util.h"
#include "ngx_http_chunkin_filter_module.h"
#include "ngx_http_chunkin_request_body.h"

enum {
    DEFAULT_MAX_CHUNKS_PER_BUF = 512
};

static ngx_int_t ngx_http_chunkin_resume_handler(ngx_http_request_t *r);

static char* ngx_http_chunkin_resume(ngx_conf_t *cf, ngx_command_t *cmd,
        void *conf);

static void *ngx_http_chunkin_create_conf(ngx_conf_t *cf);
static char *ngx_http_chunkin_merge_conf(ngx_conf_t *cf, void *parent,
    void *child);

static ngx_int_t ngx_http_chunkin_handler(ngx_http_request_t *r);

static ngx_int_t ngx_http_chunkin_init(ngx_conf_t *cf);

static void ngx_http_chunkin_post_read(ngx_http_request_t *r);


static ngx_command_t  ngx_http_chunkin_commands[] = {

    { ngx_string("chunkin"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_CONF_FLAG,
      ngx_conf_set_flag_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_chunkin_conf_t, enabled),
      NULL },

    { ngx_string("chunkin_max_chunks_per_buf"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_CONF_FLAG,
      ngx_conf_set_num_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_chunkin_conf_t, max_chunks_per_buf),
      NULL },

    { ngx_string("chunkin_resume"),
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      ngx_http_chunkin_resume,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
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
    ngx_http_chunkin_init,         /* postconfiguration */

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

    ngx_conf_merge_value(conf->enabled, prev->enabled, 0);
    ngx_conf_merge_value(conf->keepalive, prev->keepalive, 0);
    ngx_conf_merge_uint_value(conf->max_chunks_per_buf,
            prev->max_chunks_per_buf,
            DEFAULT_MAX_CHUNKS_PER_BUF);

    return NGX_CONF_OK;
}


static ngx_flag_t
ngx_http_chunkin_is_chunked_encoding(ngx_http_request_t *r)
{
    dd("is chunked encoding...");

    return r->headers_in.transfer_encoding &&
        r->headers_in.transfer_encoding->value.len >= 7 &&
        ngx_strcasestrn(r->headers_in.transfer_encoding->value.data,
                "chunked", 7 - 1);
}


static ngx_int_t
ngx_http_chunkin_init(ngx_conf_t *cf)
{
    ngx_http_handler_pt        *h;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

    h = ngx_array_push(&cmcf->phases[NGX_HTTP_ACCESS_PHASE].handlers);
    if (h == NULL) {
        return NGX_ERROR;
    }

    *h = ngx_http_chunkin_handler;

    return NGX_OK;
}


static ngx_int_t
ngx_http_chunkin_handler(ngx_http_request_t *r)
{
    ngx_http_chunkin_ctx_t      *ctx;
    ngx_http_chunkin_conf_t     *conf;
    ngx_int_t                   rc;

    dd("entered chunkin handler...");

    conf = ngx_http_get_module_loc_conf(r, ngx_http_chunkin_filter_module);

    if (!conf->enabled || r != r->main) {
        dd("conf not enabled: %d", (int) conf->enabled);

        return NGX_DECLINED;
    }

    ctx = ngx_http_get_module_ctx(r, ngx_http_chunkin_filter_module);

    if (ctx == NULL) {
        dd("ctx not found");

        return NGX_DECLINED;
    }

    if (ctx->done) {
        return NGX_DECLINED;
    }

    if (ctx->waiting_more_body) {
        return NGX_AGAIN;
    }

    dd("reading chunked input eagerly...");

    if (!conf->keepalive && r->keepalive) {
        dd("dis-enabling r->keepalive...");
        r->keepalive = 0;
    }

    dd_check_read_event_handler(r);
    dd_check_write_event_handler(r);

    /*
    rc = ngx_http_read_client_request_body(r,
            ngx_http_chunkin_post_read);
    */

    ngx_http_chunkin_clear_transfer_encoding(r);

    r->header_in->pos = r->header_end + sizeof(CRLF) - 1;

    if (r->header_in->pos > r->header_in->last) {
        ngx_log_error(NGX_LOG_INFO, r->connection->log, 0,
                      "chunkin: r->header_in->pos overflown");

        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    if (*(r->header_in->pos - 2) != CR || *(r->header_in->pos - 1) != LF) {
        ngx_log_error(NGX_LOG_INFO, r->connection->log, 0,
                      "chunkin: r->header_in->pos not lead by CRLF");

        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    dd("chunkin handler: header_in->pos: %d",
            (int)(r->header_in->pos - r->header_in->start));

    rc = ngx_http_chunkin_read_chunked_request_body(r,
            ngx_http_chunkin_post_read);

    if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        dd("read client request body returned special response %d", (int) rc);
        return rc;
    }

    dd("read client request body returned %d", (int) rc);

    if (rc == NGX_AGAIN) {
        ctx->waiting_more_body = 1;

        return NGX_AGAIN;
    }

    ctx->done = 1;

    return NGX_DECLINED;
}


static void
ngx_http_chunkin_post_read(ngx_http_request_t *r)
{
    ngx_http_chunkin_ctx_t          *ctx;

    dd("post read");

    dd_check_read_event_handler(r);
    dd_check_write_event_handler(r);

    r->read_event_handler = ngx_http_block_reading;

    ctx = ngx_http_get_module_ctx(r, ngx_http_chunkin_filter_module);

    ctx->done = 1;

    if (ctx->waiting_more_body) {
        ctx->waiting_more_body = 0;
        ngx_http_core_run_phases(r);
    }
}


static char* ngx_http_chunkin_resume(ngx_conf_t *cf, ngx_command_t *cmd,
        void *conf)
{
    ngx_http_core_loc_conf_t        *clcf;

    clcf = ngx_http_conf_get_module_loc_conf(cf,
                ngx_http_core_module);

    clcf->handler = ngx_http_chunkin_resume_handler;

    return NGX_CONF_OK;
}


static ngx_int_t
ngx_http_chunkin_resume_handler(ngx_http_request_t *r) {
    ngx_int_t                   rc;
    ngx_http_chunkin_conf_t     *conf;
    ngx_http_chunkin_ctx_t      *ctx;

    conf = ngx_http_get_module_loc_conf(r, ngx_http_chunkin_filter_module);

    dd("method: %.*s (%d)", (int) r->method_name.len, r->method_name.data,
            (int) r->method);

    if (!conf->enabled || r != r->main
            || (r->method != NGX_HTTP_PUT && r->method != NGX_HTTP_POST &&
                r->method != NGX_HTTP_DELETE))
    {
        dd("conf not enabled or in subrequest or not POST nor PUT requests");

        return NGX_HTTP_LENGTH_REQUIRED;
    }

    if (r->method == NGX_HTTP_POST &&
                ! ngx_http_chunkin_is_chunked_encoding(r->main))
    {
        dd("found POST request, but not chunked");
        return NGX_HTTP_LENGTH_REQUIRED;
    }

    dd("chunked request test passed");

    /* XXX just to fool the nginx core */
    r->headers_in.content_length_n = 1;

    ngx_http_chunkin_clear_transfer_encoding(r);

    ctx = ngx_http_get_module_ctx(r, ngx_http_chunkin_filter_module);

    if (ctx == NULL) {
        ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_chunkin_ctx_t));
        if (ctx == NULL) {
            return NGX_ERROR;
        }

        /* ctx->ignore_body is set to 0 */
    }

    r->discard_body = 0;
    r->error_page = 0;
    r->err_status = 0;

#if 0
    r->method = NGX_HTTP_PUT;
    r->headers_in.content_length = NULL;
    r->headers_in.content_length_n = -1;
#endif

    rc = ngx_http_chunkin_process_request_header(r);
    if (rc != NGX_OK) {
        return rc;
    }

#if 0
    r->plain_http = 1;
#endif

    rc = ngx_http_chunkin_process_request(r);
    if (rc != NGX_OK) {
        return rc;
    }

    return ngx_http_chunkin_internal_redirect(r, &r->main->uri, &r->main->args,
            ctx);
}

