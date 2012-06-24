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

#include "cxmap.h"

#include "pserv.h"
#include "pclient.h"

#include <glib.h>

static GHashTable *tbl;

void
cxmap_init(void)
{
    tbl = g_hash_table_new(g_str_hash, g_str_equal);
}

/* Registers a ProxyServer with a ProxyClient, creating if it exists. */
void
cxmap_register(struct ProxyServer *serv)
{
    struct ProxyClient *client = g_hash_table_lookup(tbl, serv->location);
    if (!client) {
        /* create new ProxyClient */
        struct event_base *base = bufferevent_get_base(serv->bev);
        client = pclient_new(base, serv->location, serv->path);
        g_hash_table_insert(tbl, client->location, client);
        g_debug("Creating a new ProxyClient object for %s", serv->location);
    } else {
        g_debug("Reusing an existing ProxyClient for %s", serv->location);
    }
    pclient_register(client, serv);
}

/* Removes the ProxyClient from the list of ProxyClients. */
void
cxmap_unregister(struct ProxyClient *client)
{
    g_hash_table_remove(tbl, client->location);
}
