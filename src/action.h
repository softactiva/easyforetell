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

#ifndef ACTION_H
#define	ACTION_H

#include <stdlib.h>
#include <limits.h>

#include "config_utils.h"

#ifndef ACTION_REFRESH
#define ACTION_REFRESH 1
#endif

#ifndef ARGUMENT_MAX_LENGTH
#define ARGUMENT_MAX_LENGTH 256
#endif

#ifndef VARIABLE_MAX_COUNT
#define VARIABLE_MAX_COUNT 32
#endif

#ifndef URL_MAX_COUNT
#define URL_MAX_COUNT 32
#endif

typedef struct {
    char name[ARGUMENT_MAX_LENGTH];
    char **refresh_list;
    int count;
} action_t;

action_t **create_actions(char **include_files);
void destroy_actions(action_t **actions);

int load_action(action_t **actions, const char *include_file, int index,
        const char *version);
action_t *find_action(action_t **actions, const char *name);

#endif	/* ACTION_H */
