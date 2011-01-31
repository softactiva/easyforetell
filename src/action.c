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

#include "action.h"

action_t **create_actions(char **include_files) {
    action_t **actions = NULL;
    int i;
    for (i = 0; include_files[i]; i++);
    if (i > 0) {
        actions = (action_t **) malloc(i * sizeof (action_t *));
        if (actions)
            actions[0] = NULL;
    }
    return actions;
}

void destroy_actions(action_t **actions) {
    if (actions) {
        int i;
        for (i = 0; actions[i]; i++) {
            destroy_string_list(actions[i]->refresh_list, actions[i]->count);
            free(actions[i]);
        }
        free(actions);
    }
}

action_t *create_action(const char *name) {
    action_t *action = (action_t *) malloc(sizeof (action_t));
    if (action) {
        strcpy(action->name, name);
        action->count = 0;
        action->refresh_list = NULL;
    }
    return action;
}

int load_action(action_t **actions, const char *include_file, int index,
        const char *version) {
    char *str = NULL;
    int i;

    config_t cfg;
    if (load_config(&cfg, include_file) == -1) {
        destroy_config(&cfg);
        return 0;
    }

    if (check_version(&cfg, (const char**) &str, version) == -1) {
        destroy_config(&cfg);
        return 0;
    }

    if (read_string(&cfg, "cache.name", (const char**) &str,
            ARGUMENT_MAX_LENGTH) == -1) {
        destroy_config(&cfg);
        return 0;
    }

    actions[index] = create_action(str);
    if (actions[index + 1])
        actions[index + 1] = NULL;

    log_info("Loading cache configuration: '%s'", actions[index]->name);

    int variable_count;
    char **variable_list = read_variable_list(&cfg, &variable_count,
            VARIABLE_MAX_COUNT);
    if (variable_list) {
        actions[index]->refresh_list = read_url_list(&cfg,
                &actions[index]->count, URL_MAX_COUNT,
                (const char **) variable_list,
                variable_count);
    } else {
        destroy_config(&cfg);
        return 0;
    }

    destroy_string_list(variable_list, variable_count);
    destroy_config(&cfg);
    return 1;
}

action_t *find_action(action_t **actions, const char *name) {
    action_t *action = NULL;
    int i;
    if (actions) {
        for (i = 0; !action && actions[i]; i++)
            if (!strcmp(actions[i]->name, name))
                action = actions[i];
    }
    return action;
}
