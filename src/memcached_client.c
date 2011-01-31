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

#include "memcached_client.h"

memcached_st *memcached_init() {
    return memcached_create(NULL);
}

void memcached_destroy(memcached_t *memc) {
    if (memc->servers) {
        memcached_server_list_free(memc->servers);
        memc->servers = NULL;
    }
    if (memc->memc) {
        memcached_free(memc->memc);
        memc->servers = NULL;
    }
}

int memcached_connect(memcached_t *memc, const char *servername,
        unsigned int port) {
    memcached_return res = memcached_server_add(memc->memc, servername, port);
    if (res == MEMCACHED_SUCCESS)
        return 1;
    else
        log_error("Failed to add memcached server '%s': %s", servername,
            memcached_strerror(memc->memc, res));
    return -1;
}

int memcached_connect_(memcached_t *memc, const char *server_list) {
    memc->servers = memcached_servers_parse(server_list);
    if (!memc->servers) {
        log_error("Failed to parse memcached server list");
        return -1;
    }
    memcached_return res = memcached_server_push(memc->memc, memc->servers);
    memcached_server_list_free(memc->servers);
    memc->servers = NULL;
    if (res == MEMCACHED_SUCCESS)
        return 1;
    else
        log_error("Failed to connect memcached servers from the list: %s",
            memcached_strerror(memc->memc, res));
    return -1;
}

unsigned int memcached_count_servers(const memcached_t *memc) {
    return (unsigned int) memcached_server_count(memc->memc);
}

static memcached_return server_display_function(memcached_st *memc
__attribute__((unused)), memcached_server_st *server, void *context) {
    char *context_str = (char *) context;
    log_info("%s: %s:%d", context_str, server->hostname, server->port);
    return MEMCACHED_SUCCESS;
}

void memcached_display_servers(memcached_t *memc) {
    char context[22];
    strcpy(context, "memcached server info");
    memcached_server_function callbacks[1];
    callbacks[0] = server_display_function;
    memcached_server_cursor(memc->memc, callbacks, NULL, 1);
}

int memcached_override_record(memcached_t *memc, const char *key,
        const char *value, time_t expiration) {
    memcached_return res = memcached_set(memc->memc, key, strlen(key),
            value, strlen(value), expiration, (uint32_t) 0);
    if (res == MEMCACHED_SUCCESS || res == MEMCACHED_BUFFERED) {
        log_info("Cache: %s, updated", key);
        return 1;
    } else
        log_warning("Failed to store record '%s' on memcached: %s", key,
            memcached_strerror(memc->memc, res));
    return -1;
}

int memcached_delete_record(memcached_t *memc, const char *key, int log_error) {
    memcached_return res = memcached_delete(memc->memc, key, strlen(key),
            (time_t) 0);
    if (res == MEMCACHED_SUCCESS || res == MEMCACHED_BUFFERED) {
        log_info("Cache: %s, deleted", key);
        return 1;
    } else {
        if (log_error)
            log_warning("Failed to delete record '%s' on memcached: %s", key,
                memcached_strerror(memc->memc, res));
        else
            log_info("Failed to delete record '%s' on memcached: %s", key,
                memcached_strerror(memc->memc, res));
    }
    return -1;
}

int create_key(char *out, size_t out_len, const char *prefix,
        const char *url) {
    if (out && prefix && url) {
        size_t key_length = strlen(prefix) + strlen(url);
        if (key_length < MEMCACHED_MAX_KEY && key_length <= out_len) {
            strcpy(out, prefix);
            strcat(out, url);
            return 1;
        } else
            log_error("Maximum memcached key length exceeded: %d, URL: '%s'",
                MEMCACHED_MAX_KEY, url);
    } else
        log_error("Failed to create memcached key from URL: '%s'", url);
    return -1;
}
