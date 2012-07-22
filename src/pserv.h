/*
    Copyright (C) 2012 The yummycacher Authors

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PSERV_H
#define PSERV_H

#include "protoshttp.h"

#include <event2/bufferevent.h>

enum ProxyServerState {PS_RECV, PS_SEND, PS_SENDH};

struct ProxyServer {
    /* public */

    /* private */
    struct bufferevent *bev;
    struct ProtoSHttp *proto;
    enum ProxyServerState state;
    char *location;
    char *path;

    int proxy_fd;
    int n_avail;
    int n_written;
    off_t n_tot;
};


struct ProxyServer *
pserv_new(struct bufferevent *bev);

void
pserv_free(struct ProxyServer *serv);

void
pserv_read_cb(struct bufferevent *bev, void *ctx);

void
pserv_write_cb(struct bufferevent *bev, void *ctx);

void
pserv_event_cb(struct bufferevent *bev, short events, void *ctx);

void
pserv_data_updated_cb(struct ProxyServer *serv, char *path, int n, int length);

#endif
