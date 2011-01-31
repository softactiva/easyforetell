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

#include "curl_client.h"

/* Use the callback function to download data into memory */
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *data) {
    size_t realsize = size * nmemb;
    memory_struct_t *mem = (memory_struct_t *) data;
    if (!mem->enabled)
        return realsize;

    size_t buffer_length = mem->size + realsize + 1;
    if (buffer_length > BUFFER_MEMORY_SIZE) {
        log_error("Not enough memory to download data, limit is %d bytes",
                BUFFER_MEMORY_SIZE);
        return -1;
    }

    memcpy(&(mem->memory[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = '\0';

    return realsize;
}

void curl_thread_init() {
    curl_global_init(CURL_GLOBAL_ALL);
}

int curl_init(curl_t *curl) {
    curl->buffer.memory = malloc(BUFFER_MEMORY_SIZE);
    curl->buffer.size = 0;

    curl->curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl->curl, CURLOPT_VERBOSE, 0L);
        curl_easy_setopt(curl->curl, CURLOPT_HEADER, 0L);

        /* Avoids issues with DNS timeouts in multi-threaded environments */
        curl_easy_setopt(curl->curl, CURLOPT_NOSIGNAL, 1L);
        curl_easy_setopt(curl->curl, CURLOPT_DNS_USE_GLOBAL_CACHE, 0L);

        /* Timeouts in seconds */
        curl_easy_setopt(curl->curl, CURLOPT_FTP_RESPONSE_TIMEOUT, 10L);
        curl_easy_setopt(curl->curl, CURLOPT_CONNECTTIMEOUT, 11L);
        curl_easy_setopt(curl->curl, CURLOPT_TIMEOUT, 120L);

        /* Send all data to this function */
        curl_easy_setopt(curl->curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl->curl, CURLOPT_WRITEDATA, (void *) &curl->buffer);

        /* Some servers don't attend requests that are made without a
         * user-agent */
        curl_easy_setopt(curl->curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    } else {
        log_error("Failed to create cURL");
        return -1;
    }

    return 1;
}

void curl_destroy(curl_t *curl) {
    if (curl->buffer.memory)
        free(curl->buffer.memory);
    curl_easy_cleanup(curl->curl);
}

/* Fetch the URL from the parameter url */
int curl_fetch_url(curl_t *curl, const char *url) {
    CURLcode res;

    /* Clean buffer */
    memset(curl->buffer.memory, '\0', BUFFER_MEMORY_SIZE);
    curl->buffer.size = 0;

    /* Get another document from the same server using the same
     * connection */
    curl_easy_setopt(curl->curl, CURLOPT_URL, url);
    res = curl_easy_perform(curl->curl);

    /* Handles the error */
    if (res == CURLE_OK) {
        if (curl->buffer.enabled)
            log_info("URL: %s, succeeded, %lu bytes retrieved", url,
                curl->buffer.size);
        else
            log_info("URL: %s, succeeded, fetch only", url);
        return 1;
    } else
        log_notice("URL: %s, failed with error: %s", url,
            curl_easy_strerror(res));
    return -1;
}
