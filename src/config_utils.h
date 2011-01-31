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

#ifndef CONFIG_UTILS_H
#define	CONFIG_UTILS_H

#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include <libconfig.h>

#include "logger.h"
#include "string_utils.h"

#ifndef MAX_LINE_LENGTH
#define MAX_LINE_LENGTH 256
#endif

/* # chars in a memcached key prefix, including null */
#ifndef MAX_KEY_PREFIX_LENGTH
#define MAX_KEY_PREFIX_LENGTH 13
#endif

#ifndef VAR_SEPARATOR
#define VAR_SEPARATOR ' '
#endif

int load_config(config_t *cfg, const char* cfg_filename);
void destroy_config(config_t *cfg);

int check_version(config_t *cfg, const char **version, const char *expected);

int read_pidfile_filename(const config_t *cfg, const char **pidfile_filename);
int read_socket_filename(const config_t *cfg, const char **fifo_filename);
int read_workers_count(const config_t *cfg, long int *workers_count);
int read_worker_contention(const config_t *cfg, long int *worker_contention);
char **read_memcached_servers(const config_t *cfg, int *server_count);
int read_memcached_expiration(const config_t *cfg, time_t *expiration);
int read_memcached_key_prefix(const config_t *cfg, const char **key_prefix);
int read_include_dir(const config_t *cfg, const char **include_dir);

int read_string(const config_t *cfg, const char *key, const char **value,
        int max_length);
int read_int(const config_t *cfg, const char *key, long int *value,
        long int min, long int max);

char **read_variable_list(const config_t *cfg, int *variable_count,
        int maximum_count);
char **read_url_list(const config_t *cfg, int *url_count, int maximum_count,
        const char **variables, int variable_count);

#endif	/* CONFIG_UTILS_H */
