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

#include "queue.h"

/* Initialize the queue */
void fifo_init(fifo_queue_t *queue, int capacity) {
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_mutex_lock(&queue->mutex);

    queue->capacity = capacity;
    queue->buffer = (char**) malloc(queue->capacity * sizeof (char*));
    int i;
    for (i = 0; i < queue->capacity; i++) {
        queue->buffer[i] = (char *) malloc(BUFFER_SIZE * sizeof (char));
    }

    queue->occupied = 0;
    queue->begin = 0;
    queue->end = 0;

    pthread_mutex_unlock(&queue->mutex);
}

/* Frees all resources related to the queue */
void fifo_destroy(fifo_queue_t *queue) {
    pthread_mutex_lock(&queue->mutex);
    int i;
    if (queue->occupied == 0) {
        free(queue->buffer);
    } else {
        for (i = 0; i < queue->capacity; i++) {
            free(queue->buffer[i]);
        }
        free(queue->buffer);
    }
    pthread_mutex_unlock(&queue->mutex);
    pthread_mutex_destroy(&queue->mutex);
}

/* Insert the new_element argument at the back of the queue */
void fifo_enqueue(fifo_queue_t *queue, const char *new_element, int *res) {
    pthread_mutex_lock(&queue->mutex);
    *res = -1;
    size_t len = sizeof (new_element);
    if (len <= BUFFER_SIZE && queue->occupied < queue->capacity) {
        memcpy(queue->buffer[queue->begin], new_element,
                strlen(new_element) + 1);
        queue->occupied++;
        queue->begin = (queue->begin + 1) % queue->capacity;
        *res = 1;
    }
    pthread_mutex_unlock(&queue->mutex);
}

/* Returns a reference to the value at the front of a non-empty queue */
void fifo_dequeue(fifo_queue_t *queue, char *next_element) {
    pthread_mutex_lock(&queue->mutex);
    if (queue->occupied > 0) {
        memcpy(next_element, queue->buffer[queue->end],
                strlen(queue->buffer[queue->end]) + 1);
        queue->end = (queue->end + 1) % queue->capacity;
        queue->occupied--;
    } else {
        next_element = NULL;
    }
    pthread_mutex_unlock(&queue->mutex);
}
