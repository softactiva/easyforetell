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

#include "sleep_utils.h"

int sleep_millis(long int base) {
    /* Value is in the range 0 to 'base' milliseconds) */
    int millis_to_sleep = rand() % (base + 1);
    milli_sleep((unsigned long) millis_to_sleep);
    return millis_to_sleep;
}

void milli_sleep(const unsigned long millis_to_sleep) {
    struct timespec req = {0}, rem = {0};
    time_t secs = (int) (millis_to_sleep / 1000);
    unsigned long millisecs = millis_to_sleep - (secs * 1000);
    req.tv_sec = secs;
    req.tv_nsec = millisecs * 1000000L;
    nanosleep(&req, &rem);
}
