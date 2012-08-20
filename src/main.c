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
#include "dns.h"
#include "options.h"
#include "log.h"

#include <glib.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>


/* Accept a client socket and set it up to for reading & writing */
static void
listener_accept_cb(struct evconnlistener *listener, evutil_socket_t sock,
                   struct sockaddr *addr, int len, void *ptr)
{
    struct event_base *base = evconnlistener_get_base(listener);
    struct bufferevent *bev = bufferevent_socket_new(base, sock,
                                                     BEV_OPT_CLOSE_ON_FREE);

    struct ProxyServer *serv = pserv_new(bev);
    bufferevent_setcb(bev, pserv_read_cb, pserv_write_cb, pserv_event_cb,
                      serv);
    bufferevent_enable(bev, EV_READ|EV_WRITE);

    g_debug("Received a connection");
}

/* Start the server listening on 8080 and start the first client. */
static int
start_loop(void)
{
    struct event_base *base;
    struct evconnlistener *listener;

    base = event_base_new();
    if (base == NULL) {
        g_critical("Could not open event base!");
        return 1;
    }

    dns_init(base);

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(0x7f000001);
    sin.sin_port = htons(8080);

    listener = evconnlistener_new_bind(base, listener_accept_cb, NULL,
                                       LEV_OPT_CLOSE_ON_FREE|LEV_OPT_REUSEABLE,
                                       -1, (struct sockaddr *)&sin, sizeof(sin));
    if (listener == NULL) {
        perror("Could not create listener!");
        exit(1);
    }

    g_message("Starting up main loop");
    event_base_dispatch(base);

    return 0;
}

void
handler(int signum)
{
    log_close();
    exit(0);
}

void
setup_signals(void)
{
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    if (sigaction(SIGINT, &sa, NULL) == -1)
        g_error("Error setting up signals");
}

int main(int argc, char **argv) {
    int res;
    if (!parse_options(argc, argv) || y_options.help) {
        print_usage(argv[0]);
        return 1;
    }
    setup_signals();
    log_init();
    cxmap_init();
    res = start_loop();
    log_close();
    return res;
}
