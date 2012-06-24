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
#include "protoshttp.h"

/* The maximum http request size. */
#define MAX_REQUEST_SIZE 10240

/* A template for the response. */
const char RESPONSE[] = "HTTP/1.0 200 OK\r\n"
    "Date: %s\r\n"
    "Server: yummycacher\r\n"
    "Connection: close\r\n"
    "Content-Length: %jd\r\n"
    "Content-Type: application/octet-stream\r\n\r\n";


struct ProtoSHttp *
pshttp_new(void)
{
    struct ProtoSHttp *proto = malloc(sizeof(*proto));
    /* public */

    /* private */
    proto->buf = malloc(MAX_REQUEST_SIZE);
    proto->buf[0] = '\0';

    return proto;
}

void
pshttp_free(struct ProtoSHttp *proto)
{
    free(proto->buf);
    free(proto);
}

int
pshttp_add_data(struct ProtoSHttp *proto, char *data, int n)
{
    data[n] = '\0';
    size_t n_copied = g_strlcat(proto->buf, data, MAX_REQUEST_SIZE);
    if (n_copied >= MAX_REQUEST_SIZE) {
        exit(1); // TODO
    }

    return strstr(proto->buf, "\r\n\r\n") != NULL;
}

char *
pshttp_get_location(struct ProtoSHttp *proto)
{
    /* Parse the request to get the location */
    char *start = proto->buf + 4; // "GET "
    char *end = strstr(start, " HTTP");
    *end = '\0';
    return strdup(start);
}

/* Return an HTTP formatted time string from a timestamp. */
static char *
datestr_from_time(time_t t)
{
    static char buf[200];
    struct tm *tm = gmtime(&t);
    strftime(buf, sizeof(buf), "%a, %d %b %y %T %z", tm);
    return buf;
}

char *
pshttp_get_response(struct ProtoSHttp *proto, off_t size, time_t mtime)
{
    char *buf = malloc(MAX_REQUEST_SIZE);
    snprintf(buf, MAX_REQUEST_SIZE, RESPONSE,
             datestr_from_time(mtime),
             (intmax_t)size);
    return buf;
}
