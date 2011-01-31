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

#include "string_utils.h"

char *skip_spaces(const char *str) {
    while (isspace(*str))
        ++str;
    return (char *) str;
}

/* This function included in the Linux kernel trims the string "in place" */
char *strim(char *str) {
    size_t size;
    char *end;

    str = skip_spaces(str);
    size = strlen(str);
    if (!size)
        return str;

    end = str + size - 1;
    while (end >= str && isspace(*end))
        end--;
    *(end + 1) = '\0';

    return str;
}

int replace_string(char *str, int str_length, const char *original,
        const char *replacement) {
    char buffer[str_length];
    char *found;
    if (!(found = strstr(str, original)))
        return 0;
    /* Replace avoiding buffer overflow */
    if (found - str >= str_length)
        return -1;
    strncpy(buffer, str, found - str);
    buffer[found - str] = '\0';
    if (strlen(buffer) - (found - str) + strlen(replacement)
            + strlen(found + strlen(original)) >= str_length)
        return -1;
    sprintf(buffer + (found - str), "%s%s", replacement,
            found + strlen(original));
    strcpy(str, buffer);
    return 1;
}

char **init_string_list(int count) {
    if (count < 1)
        return NULL;
    char **list = (char **) malloc(count * sizeof (char *));
    if (!list)
        return NULL;
    int i;
    for (i = 0; i < count; i++)
        list[i] = NULL;
    return list;
}

char *dup_string(const char *str) {
    size_t length = strlen(str) + 1;
    char *dup = (char*) malloc(length);
    if (dup)
        memcpy(dup, str, length);
    return dup;
}

void destroy_string_list(char **list, int count) {
    if (list) {
        int i;
        for (i = 0; i < count && list[i]; i++)
            free(list[i]);
        free(list);
    }
}
