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

#ifndef MEMCACHED_CLIENT_H
#define	MEMCACHED_CLIENT_H

#include <stdlib.h>
#include <string.h>
#include <libmemcached/memcached.h>

#include "logger.h"

typedef struct {
    struct memcached_st *memc;
    memcached_server_st *servers;
} memcached_t;

memcached_st *memcached_init();
void memcached_destroy(memcached_t *memc);

int memcached_connect(memcached_t *memc, const char *servername,
        unsigned int port);
int memcached_connect_(memcached_t *memc, const char *server_list);

unsigned int memcached_count_servers(const memcached_t *memc);
void memcached_display_servers(memcached_t *memc);

int memcached_override_record(memcached_t *memc, const char *key,
        const char *value, time_t expiration);
int memcached_delete_record(memcached_t *memc, const char *key, int log_error);

int create_key(char *out, size_t out_len, const char *prefix,
        const char *url);

#endif	/* MEMCACHED_CLIENT_H */
