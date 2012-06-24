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

#ifndef PROTOSHTTP_H
#define PROTOSHTTP_H

#include <sys/types.h>

struct ProtoSHttp {
    /* public */

    /* private */
    char *buf;
};

struct ProtoSHttp *
pshttp_new(void);

void
pshttp_free(struct ProtoSHttp *proto);

int
pshttp_add_data(struct ProtoSHttp *proto, char *data, int n);

char *
pshttp_get_location(struct ProtoSHttp *proto);

char *
pshttp_get_response(struct ProtoSHttp *proto, off_t size, time_t mtime);

#endif
