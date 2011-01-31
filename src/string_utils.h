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

#ifndef STRING_UTILS_H
#define	STRING_UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* This function included in the Linux kernel trims the string "in place" */
char *strim(char *str);

/* Provides an interface to a list that contains a fixed number of elements,
 * with methods to initialize, populate and destroy the list and the elements
 * in the list. Initially, the list is created without elements. In this way,
 * the memory used by the list can be dinamically allocated as new elements are
 * needed. In consequence, users need to indicate the last element allocated
 * in the list. To this end, the last element must be set to NULL. Otherwise,
 * the method 'destroy_string_list(char**, int)' could fail to deallocate the
 * memory used by the list, producing a runtime error. */
char **init_string_list(int count);
char *dup_string(const char *str);
void destroy_string_list(char **list, int count);

/* Replaces the first ocurrence of 'original' in the string 'str' by the
 * content in 'replacement'. The parameter 'str_length' indicates the total 
 * length (not only the positions until the null character) of the string
 * 'str'. */
int replace_string(char *str, int str_length, const char *original,
        const char *replacement);

#endif	/* STRING_UTILS_H */
