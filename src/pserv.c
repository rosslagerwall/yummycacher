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

#include "pserv.h"
#include "cxmap.h"

#include <event2/buffer.h>
#include <glib.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

void
pserv_start_transfer(struct ProxyServer *serv);

struct ProxyServer *
pserv_new(struct bufferevent *bev)
{
    struct ProxyServer *serv = malloc(sizeof(*serv));
    /* public */

    /* private */
    serv->bev = bev;
    serv->proto = pshttp_new();
    serv->state = PS_RECV;
    serv->location = NULL;
    serv->path = NULL;

    serv->proxy_fd = -1;
    serv->n_tot = 0;
    serv->n_avail = 0;
    serv->n_written = 0;
    return serv;
}

void
pserv_free(struct ProxyServer *serv)
{
    pshttp_free(serv->proto);
    if (serv->location)
        free(serv->location);
    if (serv->path)
        free(serv->path);
    if (serv->proxy_fd != -1)
        close(serv->proxy_fd);
    bufferevent_free(serv->bev);
    free(serv);

    g_debug("ProxyServer disconnect");
}

/* Adds the location to the base path and makes sure that the path is safe
   (i.e. doesn't contain any ..).
*/
static char *get_restricted_path(char *path)
{
    char *result = malloc(1024);
    char *base = "tree";
    char **pathv = g_strsplit(path, "/", 0);
    char **cur = pathv;

    /* Initialize the result with the base string and possibly a '/' char. */
    result[0] = '\0';
    strcpy(result, base);
    if (path[0] != '/') {
        strcat(result, "/");
    }

    /* Iterater through the path components, removing empty components or .. */
    while (*cur) {
        if (strcmp(*cur, "..") && strcmp(*cur, "")) {
            g_strlcat(result, "/", 1024);
            g_strlcat(result, *cur, 1024);
        }
        cur++;
    }

    g_strfreev(pathv);
    return result;
}

void
pserv_read_cb(struct bufferevent *bev, void *ctx)
{
    struct ProxyServer *serv = ctx;
    char buf[4096];
    int n, ret;
    struct evbuffer *input = bufferevent_get_input(bev);
    while ((n = evbuffer_remove(input, buf, sizeof(buf) - 1)) > 0) {
        if (serv->state == PS_RECV) {
            ret = pshttp_add_data(serv->proto, buf, n);
            if (ret) {
                pserv_start_transfer(serv);
            }
        }
    }
}

void
pserv_write_cb(struct bufferevent *bev, void *ctx)
{
    struct ProxyServer *serv = ctx;
    if (serv->state == PS_SEND) {
        struct evbuffer *output = bufferevent_get_output(serv->bev);
        if ((serv->n_avail - serv->n_written) > 0) {
            int n_towrite = MIN(32767, serv->n_avail - serv->n_written);
            int ret = evbuffer_read(output, serv->proxy_fd, n_towrite);
            serv->n_written += ret;
        } else if (serv->n_written == serv->n_tot) {
            pserv_free(serv);
        }
    }
}

void
pserv_event_cb(struct bufferevent *bev, short events, void *ctx)
{
    if (events & BEV_EVENT_ERROR) {
        perror("Error from bufferevent");
        exit(1);
    } else if (events & BEV_EVENT_EOF) {
        struct ProxyServer *serv = ctx;
        pserv_free(serv);
    }
}

void
pserv_start_transfer(struct ProxyServer *serv)
{
    serv->location = pshttp_get_location(serv->proto);
    serv->path = get_restricted_path(serv->location);

    struct stat statbuf;
    int ret = stat(serv->path, &statbuf);
    if (ret != -1 && S_ISREG(statbuf.st_mode)) {
        struct evbuffer *output = bufferevent_get_output(serv->bev);
        serv->state = PS_SEND;

        serv->proxy_fd = open(serv->path, O_RDONLY);

        char *headers = pshttp_get_response(serv->proto, statbuf.st_size,
                                            statbuf.st_mtime);
        evbuffer_add(output, headers, strlen(headers));
        free(headers);

        serv->n_tot = statbuf.st_size;
        int n_towrite = MIN(32767, serv->n_tot);
        int ret = evbuffer_read(output, serv->proxy_fd, n_towrite);
        serv->n_avail = serv->n_tot;
        serv->n_written = ret;

        g_message("Sending cached content for %s", serv->location);
    } else {
        g_message("Proxying content for %s", serv->location);
        serv->state = PS_SENDH;
        cxmap_register(serv);
    }
    return;


}

void
pserv_data_updated_cb(struct ProxyServer *serv, int n, int length)
{
    if (serv->state == PS_SENDH) {
        struct evbuffer *output = bufferevent_get_output(serv->bev);
        serv->state = PS_SEND;
        char *path = g_strconcat(serv->path, ".tmp", NULL);
        serv->proxy_fd = open(path, O_RDONLY);
        g_free(path);

        char *headers = pshttp_get_response(serv->proto, length, 1);
        evbuffer_add(output, headers, strlen(headers));
        free(headers);

        serv->n_tot = length;
    }

    if (serv->state == PS_SEND) {
        struct evbuffer *output = bufferevent_get_output(serv->bev);
        serv->n_avail = n;
        if (evbuffer_get_length(output) == 0) {
            int n_towrite = MIN(32767, serv->n_avail - serv->n_written);
            int ret = evbuffer_read(output, serv->proxy_fd, n_towrite);
            serv->n_written += ret;
        }
    }
}
