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

#ifndef CURL_CLIENT_H
#define	CURL_CLIENT_H

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "logger.h"

/* Maximum allowed size in bytes for Web content */
#ifndef BUFFER_MEMORY_SIZE
#define BUFFER_MEMORY_SIZE 102400
#endif

typedef struct {
    char *memory;
    size_t size;
    int enabled;
} memory_struct_t;

typedef struct {
    CURL *curl;
    memory_struct_t buffer;    
} curl_t;

void curl_thread_init();
int curl_init(curl_t *curl);
void curl_destroy(curl_t *curl);
int curl_fetch_url(curl_t *curl, const char *url);

#endif	/* CURL_CLIENT_H */
