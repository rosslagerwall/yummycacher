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

#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <stdint.h>
#include "protochttp.h"

/* The maximum http request size. */
#define MAX_REQUEST_SIZE 10240

/* A template for the request. */
const char REQUEST[] = "GET %s HTTP/1.0\r\n"
    "User-Agent: yummycacher\r\n"
    "Host: archive.fedoraproject.org\r\n\r\n";


struct ProtoCHttp *
pchttp_new(void)
{
    struct ProtoCHttp *proto = malloc(sizeof(*proto));
    /* public */

    /* private */
    proto->buf = malloc(MAX_REQUEST_SIZE);
    proto->buf[0] = '\0';
    proto->bufsize = 0;

    return proto;
}

void
pchttp_free(struct ProtoCHttp *proto)
{
    free(proto->buf);
    free(proto);
}

int
pchttp_add_data(struct ProtoCHttp *proto, char *data, int n, char **out)
{
    int n_remaining = MAX_REQUEST_SIZE - proto->bufsize - 1;
    memcpy(proto->buf, data, MIN(n_remaining, n));
    proto->bufsize += MIN(n_remaining, n);
    proto->buf[proto->bufsize] = '\0';

    char *ptr;
    if ((ptr = strstr(proto->buf, "\r\n\r\n")) != NULL) {
        /* parse the headers */
        ptr[0] = '\0';
///        http_request_parse_headers(req);

        ptr += 4;
        size_t count = proto->bufsize - (ptr - proto->buf);
        *out = malloc(count);
        memcpy(*out, ptr, count);
        return count;
    }
    return 0;
}

int
pchttp_get_length(struct ProtoCHttp *proto)
{
    char **lines = g_strsplit(proto->buf, "\r\n", 0);
    char **lineptr = lines;
    int length = 0;
    while (*lineptr) {
        if (g_str_has_prefix(*lineptr, "Content-Length:")) {
            puts(*lineptr);
            sscanf(*lineptr, "Content-Length: %d", &length);
        }

        lineptr++;
    }
    g_strfreev(lines);

    return length;
}

char *
pchttp_get_request(struct ProtoCHttp *proto, char *location)
{
    char *buf = malloc(MAX_REQUEST_SIZE);
    snprintf(buf, MAX_REQUEST_SIZE, REQUEST,
             location);
    return buf;
}
