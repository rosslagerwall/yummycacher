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

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <event2/buffer.h>
#include "pclient.h"
#include "dns.h"

void
pclient_read_cb(struct bufferevent *bev, void *ctx);
void
pclient_write_cb(struct bufferevent *bev, void *ctx);
void
pclient_event_cb(struct bufferevent *bev, short events, void *ctx);

/* Implements a mkdir -p routine. */
void mkdir_p(char *path)
{
    char *pathbuf = malloc(strlen(path) + 1);
    pathbuf[0] = '\0';
    char **components = g_strsplit(path, "/", 0);
    int n_components = g_strv_length(components) - 1;
    int i;

    for (i = 0; i < n_components; i++) {
        strcat(pathbuf, components[i]);
        strcat(pathbuf, "/");
        mkdir(pathbuf, 0777);
    }

    g_strfreev(components);
    free(pathbuf);
}

struct ProxyClient *
pclient_new(struct event_base *base, char *location, char *path)
{
    struct ProxyClient *client = malloc(sizeof(*client));
    /* public */

    /* private */
    client->n_written = 0;
    client->observers = NULL;
    client->proto = pchttp_new();
    client->state = PC_SEND;
    client->location = strdup(location);
    client->path = g_strconcat(path, ".tmp", NULL);
    client->orig_path = strdup(path);
    mkdir_p(path);
    client->sink = fopen(client->path, "w");

    /* Create a socket to connect to the remote http server */
    client->bev = bufferevent_socket_new(base, -1,
                                         BEV_OPT_CLOSE_ON_FREE);
    bufferevent_setcb(client->bev, pclient_read_cb, pclient_write_cb,
                      pclient_event_cb, client);
    bufferevent_enable(client->bev, EV_READ|EV_WRITE);

    /* Make the reuqest for the file */
    struct evbuffer *output = bufferevent_get_output(client->bev);
    char *buf = pchttp_get_request(client->proto, location);
    evbuffer_add(output, buf, strlen(buf));
    free(buf);

    /* Schedule the connection with libevent */
    bufferevent_socket_connect_hostname(
        client->bev, dns_base, AF_UNSPEC, "archive.fedoraproject.org", 80);

    return client;
}

void
pclient_free(struct ProxyClient *client)
{
    rename(client->path, client->orig_path);
    pchttp_free(client->proto);
    free(client->location);
    g_free(client->path);
    free(client->orig_path);
    bufferevent_free(client->bev);
    fclose(client->sink);
    free(client);

    g_debug("ProxyClient disconnect");
}

void
pclient_notify_all_data(struct ProxyClient *client, int n, int length)
{
    GList *cur = client->observers;
    while (cur != NULL) {
        pserv_data_updated_cb(cur->data, n, length);
        cur = cur->next;
    }
}

void
pclient_read_cb(struct bufferevent *bev, void *ctx)
{
    struct ProxyClient *client = ctx;
    char buf[4096];
    int n, ret;
    struct evbuffer *input = bufferevent_get_input(bev);
    while ((n = evbuffer_remove(input, buf, sizeof(buf))) > 0) {
        /* If we're still receiving the headers, pass the data through
           ProtoCHttp, else write the data directly to the sink. */
        if (client->state == PC_SEND) {
            char *out;
            ret = pchttp_add_data(client->proto, buf, n, &out);
            if (ret) {
                fwrite(out, 1, ret, client->sink);
                fflush(client->sink);
                client->n_written = ret;
                pclient_notify_all_data(client, client->n_written, client->proto->http_length);
                free(out);
                client->state = PC_RECV;
            }
        } else {
            fwrite(buf, 1, n, client->sink);
            fflush(client->sink);
            client->n_written += n;
            pclient_notify_all_data(client, client->n_written, client->proto->http_length);
        }
    }
}

void
pclient_write_cb(struct bufferevent *bev, void *ctx)
{
}

void
pclient_event_cb(struct bufferevent *bev, short events, void *ctx)
{
    if (events & BEV_EVENT_ERROR) {
        perror("Error from bufferevent");
        exit(1);
    } else if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR)) {
        struct ProxyClient *client = ctx;
        pclient_free(client);
    }
}

void
pclient_register(struct ProxyClient *client, struct ProxyServer *serv)
{
    client->observers = g_list_prepend(client->observers, serv);
}
