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

#include "config_utils.h"

int load_config(config_t *cfg, const char* cfg_filename) {
    config_init(cfg);
    if (!config_read_file(cfg, cfg_filename)) {
        log_error("%s:%d - %s", cfg_filename, cfg->error_line,
                cfg->error_text);
        config_destroy(cfg);
        return -1;
    }
    return 1;
}

void destroy_config(config_t *cfg) {
    config_destroy(cfg);
}

int check_version(config_t *cfg, const char **version, const char *expected) {
    if (!config_lookup_string(cfg, "version", version)) {
        log_error("No 'version' setting in configuration file");
        return -1;
    } else if (strcmp(expected, *version)) {
        log_error("Expected configuration version '%s', but '%s' found",
                expected, *version);
        return -1;
    }
    return 1;
}

int read_pidfile_filename(const config_t *cfg, const char **pidfile_filename) {
    if (!config_lookup_string(cfg, "pidfile.filename", pidfile_filename)) {
        log_info("No 'pidfile.filename' setting in configuration file");
        return -1;
    } else if (!pidfile_filename || strlen(*pidfile_filename) >= PATH_MAX) {
        log_warning("Maximum length exceeded on 'pidfile.filename': %d",
                PATH_MAX - 1);
        return -1;
    }
    return 1;
}

int read_socket_filename(const config_t *cfg, const char **fifo_filename) {
    if (!config_lookup_string(cfg, "socket.filename", fifo_filename)) {
        log_info("No 'socket.filename' setting in configuration file");
        return -1;
    } else if (!fifo_filename || strlen(*fifo_filename) >= PATH_MAX) {
        log_warning("Maximum length exceeded on 'socket.filename': %d",
                PATH_MAX - 1);
        return -1;
    }
    return 1;
}

int read_workers_count(const config_t *cfg, long int *workers_count) {
    if (!config_lookup_int(cfg, "worker.threads", workers_count)) {
        log_info("No 'worker.threads' setting in configuration file");
        return -1;
    } else if (workers_count < 0L) {
        log_warning("Invalid 'worker.threads' in configuration file");
        return -1;
    }
    return 1;
}

int read_worker_contention(const config_t *cfg, long int *worker_contention) {
    if (!config_lookup_int(cfg, "worker.contention", worker_contention)) {
        log_info("No 'worker.contention' setting in configuration file");
        return -1;
    } else if (worker_contention < 0L) {
        log_warning("Invalid 'worker.contention' in configuration file");
        return -1;
    }
    return 1;
}

char **read_memcached_servers(const config_t *cfg, int *server_count) {
    char **memcached_servers = NULL;
    config_setting_t *setting = config_lookup(cfg, "memcached.servers");
    if (!setting) {
        log_info("No 'memcached.servers' setting in configuration file");
        return memcached_servers;
    }
    *server_count = config_setting_length(setting);
    memcached_servers = init_string_list(*server_count);
    const char *hostname;
    char tmp[MAX_LINE_LENGTH];
    long int port;
    int i, idx = 0;
    for (i = 0; i < *server_count; ++i) {
        config_setting_t *server = config_setting_get_elem(setting, i);
        /* Only store the record if all of the expected fields are present */
        if (!(config_setting_lookup_string(server, "hostname", &hostname)
                && config_setting_lookup_int(server, "port", &port))) {
            log_warning("memcached servers incomplete");
            continue;
        }
        sprintf(tmp, "%s:%ld", hostname, port);
        if (!(memcached_servers[idx] = dup_string(tmp))) {
            log_error("Failed to allocate memory for memcached server names");
            destroy_string_list(memcached_servers, *server_count);
            return NULL;
        }
        idx++;
    }
    return memcached_servers;
}

int read_memcached_expiration(const config_t *cfg, time_t * expiration) {
    long int value;
    if (!config_lookup_int(cfg, "memcached.expiration", &value)) {
        log_info("No 'memcached.expiration' setting in configuration file");
        return -1;
    } else if (value < 0L) {
        log_warning("Invalid 'memcached.expiration' in configuration file");
        return -1;
    }
    *expiration = (time_t) value;
    return 1;
}

int read_memcached_key_prefix(const config_t *cfg, const char **key_prefix) {
    if (!config_lookup_string(cfg, "memcached.key.prefix", key_prefix)) {
        log_info("No 'memcached.key.prefix' setting in configuration file");
        return -1;
    } else if (!key_prefix || strlen(*key_prefix) >= MAX_KEY_PREFIX_LENGTH) {
        log_warning("Maximum length exceeded on 'memcached.key.prefix': %d",
                MAX_KEY_PREFIX_LENGTH - 1);
        return -1;
    }
    return 1;
}

int read_include_dir(const config_t *cfg, const char **include_dir) {
    if (!config_lookup_string(cfg, "include_dir", include_dir)) {
        log_info("No 'include_dir' setting in configuration file");
        return -1;
    } else if (!include_dir || strlen(*include_dir) >= PATH_MAX) {
        log_warning("Maximum length exceeded on 'include_dir': %d",
                PATH_MAX - 1);
        return -1;
    }
    return 1;
}

int read_string(const config_t *cfg, const char *key, const char **value,
        int max_length) {
    if (!config_lookup_string(cfg, key, value)) {
        log_error("No '%s' setting in configuration file", key);
        return -1;
    } else if (!value || strlen(*value) > max_length) {
        log_error("Maximum length exceeded on '%s': %d", key, max_length);
        return -1;
    }
    return 1;
}

int read_int(const config_t *cfg, const char *key, long int *value,
        long int min, long int max) {
    long int read_value;
    if (!config_lookup_int(cfg, key, &read_value)) {
        log_info("No '%s' setting in configuration file", key);
        return -1;
    } else if (read_value < min && read_value > max) {
        log_warning("Invalid '%s' in configuration file", key);
        return -1;
    }
    *value = read_value;
    return 1;
}

char **read_variable_list(const config_t *cfg, int *variable_count,
        int maximum_count) {
    char **variables_list = NULL;
    config_setting_t *setting = config_lookup(cfg, "cache.variables");
    if (!setting) {
        log_info("No 'cache.variables' setting in configuration file");
        return variables_list;
    }
    *variable_count = config_setting_length(setting);
    if (*variable_count > maximum_count) {
        log_info("Maximum number of variables exceeded in configuration: %d",
                maximum_count);
        return variables_list;
    }
    variables_list = init_string_list(*variable_count);
    const char *name, *value, *found;
    char tmp[MAX_LINE_LENGTH];
    int i, idx = 0;
    for (i = 0; i < *variable_count; ++i) {
        config_setting_t *variable = config_setting_get_elem(setting, i);
        /* Only store the record if all of the expected fields are present */
        if (!(config_setting_lookup_string(variable, "name", &name)
                && config_setting_lookup_string(variable, "value", &value))) {
            log_warning("Cache variables incomplete");
            continue;
        }
        found = strchr(name, ' ');
        if (found) {
            log_warning("Found invalid name while parsing variables: '%s'",
                    name);
            continue;
        }
        sprintf(tmp, "$%s%c%s", name, VAR_SEPARATOR, value);
        if (!(variables_list[idx] = dup_string(tmp))) {
            log_error("Failed to allocate memory for cache variables");
            destroy_string_list(variables_list, *variable_count);
            return NULL;
        }
        idx++;
    }
    return variables_list;
}

void substitute_variables(char *str, int str_length, const char **variables,
        int variable_count) {
    int i, replaced;
    char token[2], variable[MAX_LINE_LENGTH], name[MAX_LINE_LENGTH];
    sprintf(token, "%c", VAR_SEPARATOR);
    for (i = 0; i < variable_count && variables[i]; i++) {
        strcpy(variable, variables[i]);
        /* Get the name of the variable */
        char *ptok = strtok(variable, token);
        if (ptok != NULL) {
            strcpy(name, ptok);
            /* Get the value of the variable */
            ptok = strtok(NULL, token);
            if (ptok != NULL)
                memmove(variable, ptok, str_length);
        }

        /* Replace all the occurences of the variable in the string */
        do {
            replaced = (replace_string(str, str_length, name, variable) == 1);
        } while (replaced);
    }
}

char **read_url_list(const config_t *cfg, int *url_count, int maximum_count,
        const char **variables, int variable_count) {
    char **urls_list = NULL;
    config_setting_t *setting = config_lookup(cfg, "cache.pages");
    if (!setting) {
        log_info("No 'cache.pages' setting in configuration file");
        return urls_list;
    }
    *url_count = config_setting_length(setting);
    if (*url_count > maximum_count) {
        log_info("Maximum number of pages exceeded in configuration: %d",
                maximum_count);
        return urls_list;
    }
    urls_list = init_string_list(*url_count);
    const char *url;
    char tmp[MAX_LINE_LENGTH];
    int i, idx = 0;
    for (i = 0; i < *url_count; ++i) {
        config_setting_t *variable = config_setting_get_elem(setting, i);
        /* Only store the record if all of the expected fields are present */
        if (!(config_setting_lookup_string(variable, "url", &url))) {
            log_warning("Cache pages incomplete");
            continue;
        }
        strcpy(tmp, url);
        substitute_variables(tmp, MAX_LINE_LENGTH, variables, variable_count);
        if (!(urls_list[idx] = dup_string(tmp))) {
            log_error("Failed to allocate memory for cache pages");
            destroy_string_list(urls_list, *url_count);
            return NULL;
        }
        idx++;
    }
    return urls_list;
}
