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

#ifndef DAEMON_UTILS_H
#define	DAEMON_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#include <sys/resource.h>
#include <sys/stat.h>

#include "logger.h"

#ifndef DAEMON_WORKING_DIR
#define DAEMON_WORKING_DIR "/"
#endif

#ifndef DAEMON_STDIN_FILE
#define DAEMON_STDIN_FILE "/dev/null"
#endif

#ifndef DAEMON_STDOUT_FILE
#define DAEMON_STDOUT_FILE "/dev/null"
#endif

#ifndef DAEMON_STDERR_FILE
#define DAEMON_STDERR_FILE "/dev/null"
#endif

int daemonize(const char *pidfile);

#endif	/* DAEMON_UTILS_H */
