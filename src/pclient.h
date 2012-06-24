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

#ifndef PCLIENT_H
#define PCLIENT_H

#include "protochttp.h"
#include "pserv.h"

#include <glib.h>
#include <event2/event.h>
#include <event2/bufferevent.h>

#include <stdio.h>

enum ProxyClientState {PC_RECV, PC_SEND};

struct ProxyClient {
    /* public */

    /* private */
    struct bufferevent *bev;
    struct ProtoCHttp *proto;
    enum ProxyClientState state;
    char *location;
    char *path;
    char *orig_path;
    FILE *sink;
    GList *observers;
    int length;
    int n_written;
};


struct ProxyClient *
pclient_new(struct event_base *base, char *location, char *path);

void
pclient_free(struct ProxyClient *client);

void
pclient_register(struct ProxyClient *client, struct ProxyServer *serv);

#endif
