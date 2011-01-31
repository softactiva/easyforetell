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

#ifndef QUERAL_H
#define	QUERAL_H

//#define DEBUG
#define VERBOSE 1

#ifdef DEBUG
#if VERBOSE >= 2
#define WHERESTR  "[file %s, line %d]\n"
#define WHEREARG  __FILE__, __LINE__
#define DEBUGPRINT2(...) fprintf(stderr, __VA_ARGS__)
#define DEBUGPRINT(_fmt, ...) DEBUGPRINT2(WHERESTR _fmt, WHEREARG, __VA_ARGS__)
#else
#define DEBUGPRINT2(...) fprintf(stderr, __VA_ARGS__)
#define DEBUGPRINT(_fmt, ...) DEBUGPRINT2(_fmt, __VA_ARGS__)
#endif
#else
#define DEBUGPRINT(_fmt, ...) /* do nothing */
#endif

#ifndef FETCH
#define FETCH 1
#endif

#ifndef CACHE
#define CACHE 2
#endif

#ifndef UPDATE
#define UPDATE 3
#endif

#ifndef UNDEFINED
#define UNDEFINED -1
#endif

#ifndef FETCH_COMMAND
#define FETCH_COMMAND "FETCH"
#endif

#ifndef CACHE_COMMAND
#define CACHE_COMMAND "CACHE"
#endif

#ifndef UPDATE_COMMAND
#define UPDATE_COMMAND "UPDATE"
#endif

#endif	/* QUERAL_H */
