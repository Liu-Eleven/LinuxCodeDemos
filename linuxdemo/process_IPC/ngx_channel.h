
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_CHANNEL_H_INCLUDED_
#define _NGX_CHANNEL_H_INCLUDED_


#include "ngx_process.h"


typedef struct {
    ngx_uint_t  command;
    ngx_pid_t   pid;
    ngx_int_t   slot;
    int    fd;
} ngx_channel_t;


ngx_int_t ngx_write_channel(int s, ngx_channel_t *ch, size_t size);
ngx_int_t ngx_read_channel(int s, ngx_channel_t *ch, size_t size);
//ngx_int_t ngx_add_channel_event(ngx_cycle_t *cycle, ngx_fd_t fd,
//    ngx_int_t event, ngx_event_handler_pt handler);
void ngx_close_channel(int *fd);


#endif /* _NGX_CHANNEL_H_INCLUDED_ */
