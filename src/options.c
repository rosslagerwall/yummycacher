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

#include "options.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

struct y_options y_options;

int
parse_options(int argc, char **argv)
{
    int c;

    /* Set defaults */
    y_options.root = ".";
    y_options.url = "";
    y_options.port = "8080";
    y_options.log = "-";
    y_options.foreground = 0;
    y_options.help = 0;

    for (;;) {
        int option_index = 0;
        static struct option long_options[] = {
            {"root", required_argument, NULL, 0},
            {"log", required_argument, NULL, 0},
            {"port", required_argument, NULL, 0},
            {"url", required_argument, NULL, 0},
            {"foreground", no_argument, NULL, 0},
            {"help", no_argument, NULL, 0},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "r::u:p::l::fh", long_options,
                        &option_index);
        if (c == -1)
            break;

        switch (c) {
        case 0:
            switch (option_index) {
            case 0:
                y_options.root = optarg;
                break;
            case 1:
                y_options.log = optarg;
                break;
            case 2:
                y_options.port = optarg;
                break;
            case 3:
                y_options.url = optarg;
                break;
            case 4:
                y_options.foreground = 1;
                break;
            case 5:
                y_options.help = 1;
            default:
                break;
            }
            break;
        case 'r':
            y_options.root = optarg;
            break;
        case 'l':
            y_options.log = optarg;
            break;
        case 'p':
            y_options.port = optarg;
            break;
        case 'u':
            y_options.url = optarg;
            break;
        case 'f':
            y_options.foreground = 1;
            break;
        case 'h':
            y_options.help = 1;
            break;
        case '?':
        default:
            return 0;
        }
    }

    /* Check for required options */
    if (!strcmp(y_options.url, ""))
        return 0;

    return 1;
}

void
print_usage(const char *program)
{
    fprintf(stderr, "usage: %s [options] -u <url>\n", program);
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, " -h, --help          this help text\n");
    fprintf(stderr, " -f, --foreground    don't daemonize\n");
    fprintf(stderr, " -r, --root <root>   use <root> to store cached content"
            " (default: .)\n");
    fprintf(stderr, " -l, --log <logfile> log to <logfile> (default -,"
            " stdin)\n");
    fprintf(stderr, " -p, --port <port>   listen on <port> (default 8080)\n");
    fprintf(stderr, " -u, --url <url>     mirror content for <url>\n");
}
