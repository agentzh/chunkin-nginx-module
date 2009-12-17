/* Copyright (C) agentzh */

#define DDEBUG 0
#include "ddebug.h"

#include <ngx_config.h>

#include "ngx_http_chunkin_util.h"
#include "ngx_http_chunkin_filter_module.h"
#include "ngx_http_chunkin_request_body.h"

static ngx_http_output_header_filter_pt  ngx_http_next_header_filter;

static ngx_http_output_body_filter_pt    ngx_http_next_body_filter;

static ngx_int_t ngx_http_chunkin_header_filter(ngx_http_request_t *r);
static ngx_int_t ngx_http_chunkin_body_filter(ngx_http_request_t *r,
        ngx_chain_t *in);

static void *ngx_http_chunkin_create_conf(ngx_conf_t *cf);
static char *ngx_http_chunkin_merge_conf(ngx_conf_t *cf, void *parent,
    void *child);
static ngx_int_t ngx_http_chunkin_filter_init(ngx_conf_t *cf);

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

    return conf;
}


static char *
ngx_http_chunkin_merge_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_chunkin_conf_t *prev = parent;
    ngx_http_chunkin_conf_t *conf = child;

    ngx_conf_merge_value(conf->enabled, prev->enabled, 0);
    ngx_conf_merge_value(conf->keepalive, prev->keepalive, 0);

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
ngx_http_chunkin_is_chunked_encoding(ngx_http_request_t *r)
{
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

    if (r != r->main || r->request_body) {
        dd("We ignore subrequests.");
        return ngx_http_next_header_filter(r);
    }

    dd("header filter: header_in->pos: %d", (int)(r->header_in->pos - r->header_in->start));

    conf = ngx_http_get_module_loc_conf(r, ngx_http_chunkin_filter_module);

    /*
    dd("chunkin enabled? %d", conf->enabled);
    dd("length required status? %d",
            r->headers_out.status == NGX_HTTP_LENGTH_REQUIRED);
    dd("is chunked? %d",
            ngx_http_chunkin_is_chunked_encoding(r));
            */

    if ( ! conf->enabled || r->headers_out.status != NGX_HTTP_LENGTH_REQUIRED
            || ! ngx_http_chunkin_is_chunked_encoding(r))
    {
        dd("inactive");
        return ngx_http_next_header_filter(r);
    }

    ctx = ngx_http_get_module_ctx(r, ngx_http_chunkin_filter_module);

    if (ctx) {
        dd("HAHA Found ctx...skipped.");
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

    ctx = ngx_http_get_module_ctx(r, ngx_http_chunkin_filter_module);

    if (ctx == NULL || !ctx->ignore_body || r->request_body) {
        dd("ctx NULL? %s", ctx == NULL ? "yes" : "NO");
        return ngx_http_next_body_filter(r, in);
    }

    /* throw away the 411 response body */

    last = 0;

    for (cl = in; cl; cl = cl->next) {
        /* mark the buf as consumed */
        cl->buf->pos = cl->buf->last;

        if (cl->buf->last_buf) {
            last = 1;
        }
    }

    if (last) {
        dd("ignore last body...");

        ctx->ignore_body = 0;

        ctx->r_discard_body = r->discard_body;
        r->discard_body = 0;
        r->error_page = 0;
        r->err_status = 0;

        /* set new content length header */
        rc = ngx_http_chunkin_set_content_length_header(r,
                sizeof("0\r\n\r\n"));
        if (rc != NGX_OK) {
            ngx_log_error(NGX_LOG_INFO, r->connection->log, 0,
                          "chunkin: failed to set content length.");

            return NGX_OK;
        }

        if (r->headers_in.transfer_encoding) {
            r->headers_in.transfer_encoding->value.len = 0;
            r->headers_in.transfer_encoding->value.data = (u_char*) "";
        }

        /*
        r->method = NGX_HTTP_PUT;
        r->headers_in.content_length = NULL;
        r->headers_in.content_length_n = -1;
        */

        rc = ngx_http_chunkin_process_request_header(r);
        if (rc != NGX_OK) {
            return rc;
        }

        /* r->plain_http = 1; */

        rc = ngx_http_chunkin_process_request(r);
        if (rc != NGX_OK) {
            return rc;
        }

        return ngx_http_chunkin_restart_request(r, ctx);
    }

    dd("ignore body...");

    return NGX_OK;
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

    return ngx_http_chunkin_filter_init(cf);
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
        return NGX_DECLINED;
    }

    ctx = ngx_http_get_module_ctx(r, ngx_http_chunkin_filter_module);

    if (ctx == NULL) {
        return NGX_DECLINED;
    }

    dd("reading chunked input eagerly...");

    /* XXX this is a hack for now */

    if (conf->keepalive
            && r->headers_in.connection_type ==
                NGX_HTTP_CONNECTION_KEEP_ALIVE) {
        dd("re-enable r->keepalive...");
        r->keepalive = 1;
    } else {
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

    dd("chunkin handler: header_in->pos: %d", (int)(r->header_in->pos - r->header_in->start));

    rc = ngx_http_chunkin_read_chunked_request_body(r,
            ngx_http_chunkin_post_read);

    if (rc >= NGX_HTTP_SPECIAL_RESPONSE) {
        dd("read client request body returned special response %d", rc);
        return rc;
    }

    dd("read client request body returned %d", rc);

    if (rc == NGX_OK) {
        return NGX_DECLINED;
    }

    return rc;
}


static void
ngx_http_chunkin_post_read(ngx_http_request_t *r)
{
    /* ngx_http_chunkin_ctx_t          *ctx; */

    dd("post read");

    dd_check_read_event_handler(r);
    dd_check_write_event_handler(r);

    r->read_event_handler = ngx_http_block_reading;
    r->write_event_handler = ngx_http_core_run_phases;

    /*
    ctx = ngx_http_get_module_ctx(r, ngx_http_chunkin_filter_module);

    if (ctx == NULL) {
        ctx = ngx_pcalloc(r->pool, sizeof(ngx_http_chunkin_ctx_t));
        if (ctx == NULL) {
            ngx_http_core_run_phases(r);
            return;
        }
    }

    dd("set ignore_body in post read");

    */

    r->phase_handler++;
    ngx_http_core_run_phases(r);
}

