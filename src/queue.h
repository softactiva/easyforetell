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

#ifndef QUEUE_H
#define	QUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 255
#endif

typedef struct {
    char** buffer;
    int capacity;
    int occupied;
    int begin;
    int end;
    pthread_mutex_t mutex;
} fifo_queue_t;

void fifo_init(fifo_queue_t *queue, int capacity);
void fifo_destroy(fifo_queue_t *queue);
void fifo_enqueue(fifo_queue_t *queue, const char *new_element, int *res);
void fifo_dequeue(fifo_queue_t *queue, char *next_element);

#endif	/* QUEUE_H */
