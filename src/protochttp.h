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

#ifndef PROTOCHTTP_H
#define PROTOCHTTP_H

struct ProtoCHttp {
    /* public */

    /* private */
    char *buf;
    int bufsize;
};

struct ProtoCHttp *
pchttp_new(void);

void
pchttp_free(struct ProtoCHttp *proto);

int
pchttp_add_data(struct ProtoCHttp *proto, char *data, int n, char **out);

int
pchttp_get_length(struct ProtoCHttp *proto);

char *
pchttp_get_request(struct ProtoCHttp *proto, char *location);

#endif
