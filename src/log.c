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

#include "log.h"
#include "options.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>

FILE *log_file;

void
log_handler(const char *log_domain, GLogLevelFlags log_level,
            const char *message, gpointer user_data)
{
    fprintf(log_file, "%s\n", message);
}

/* Set up logging */
void
log_init(void)
{
    if (strcmp(y_options.log, "-")) {
        log_file = fopen(y_options.log, "a");
        setlinebuf(log_file);
        g_log_set_handler(NULL, G_LOG_LEVEL_MASK, log_handler, NULL);
    }
}

void
log_close(void)
{
    if (log_file != NULL)
        fclose(log_file);
}
