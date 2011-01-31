/**
 * EasyForetell, The Predictive Cache for Advanced Web Applications.
 *
 * Copyright (C) 2010 - 2011 Erik Torres Serrano
 *
 * If you need assistance with this program, you may contact:
 *
 * http://softactiva.com or email etorres@softactiva.com.
 *
 * This file is part of EasyForetell.
 *
 * EasyForetell is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * EasyForetell is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EasyForetell. If not, see <http://www.gnu.org/licenses/>.
 */

#include "logger.h"

#include <stdio.h>

void log_init(int verbose) {
    /* Sets the logmask so that debug and informational messages get discarded
     * without ever reaching Syslog */
    if (!verbose)
        setlogmask(LOG_UPTO(LOG_NOTICE));
}

void log_init_verbose() {
    log_init(1);
}

void log_init_quiet() {
    log_init(0);
}

void log_message(int facility_priority, char *message) {
    openlog(QUERAL_LOGGER, LOG_CONS | LOG_PID | LOG_NDELAY, LOG_DAEMON);
    syslog(facility_priority, "%s", message);
    closelog();
}

void log_info(char *format, ...) {
    va_list arguments;
    va_start(arguments, format);

    char buffer[QUERAL_LOG_BUFFER_SIZE];
    vsprintf(buffer, format, arguments);
    va_end(arguments);

    log_message(LOG_INFO, buffer);
}

void log_notice(char *format, ...) {
    va_list arguments;
    va_start(arguments, format);

    char buffer[QUERAL_LOG_BUFFER_SIZE];
    vsprintf(buffer, format, arguments);
    va_end(arguments);

    log_message(LOG_NOTICE, buffer);
}

void log_warning(char *format, ...) {
    va_list arguments;
    va_start(arguments, format);

    char buffer[QUERAL_LOG_BUFFER_SIZE];
    vsprintf(buffer, format, arguments);
    va_end(arguments);

    log_message(LOG_WARNING, buffer);
}

void log_error(char *format, ...) {
    va_list arguments;
    va_start(arguments, format);

    char buffer[QUERAL_LOG_BUFFER_SIZE];
    vsprintf(buffer, format, arguments);
    va_end(arguments);

    log_message(LOG_ERR, buffer);
}
