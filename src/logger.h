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

#ifndef LOGGER_H
#define	LOGGER_H

#include <stdlib.h>
#include <syslog.h>
#include <stdarg.h>

#ifndef QUERAL_LOGGER
#define QUERAL_LOGGER "queral"
#endif

#ifndef QUERAL_LOG_BUFFER_SIZE
#define QUERAL_LOG_BUFFER_SIZE 256
#endif

void log_init_verbose();
void log_init_quiet();

void log_info(char *format, ...);
void log_notice(char *format, ...);
void log_warning(char *format, ...);
void log_error(char *format, ...);

#endif	/* LOGGER_H */
