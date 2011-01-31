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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>

#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>

#include <pthread.h>
#include <ev.h>

#include "action.h"
#include "config_utils.h"
#include "curl_client.h"
#include "file_utils.h"
#include "logger.h"
#include "memcached_client.h"
#include "queral.h"
#include "queue.h"
#include "sleep_utils.h"
#include "string_utils.h"

fifo_queue_t queue;
#define fifo_init(capacity) fifo_init(&queue, capacity)
#define fifo_destroy() fifo_destroy(&queue)
#define fifo_enqueue(new_element, res) fifo_enqueue(&queue, new_element, res)
#define fifo_dequeue(res) fifo_dequeue(&queue, res)

#define QUEUE_CAPACITY 256

#define CONFIG_FILENAME "/etc/queral.conf"
#define DEFAULT_FIFO_FILENAME "/var/run/queral/queral.fifo"
#define DEFAULT_PIDFILE_FILENAME "/var/run/queral/queral.pid"
#define DEFAULT_MEMCACHED_KEY_PREFIX "__memc_"
#define DEDAULT_CONF_FILE_EXTENSION ".conf"

#define DEFAULT_MEMCACHE_EXPIRATION_SECONDS 7200

#define DEFAULT_WORKERS 2
#define MAX_WORKERS 8
#define DEFAULT_WORKER_CONTENTION_MILLIS 100
#define MAX_WORKER_CONTENTION_MILLIS 600000

#define QUERAL_VERSION "1.0"

typedef struct {
    int worker_id;
    curl_t curl;
    memcached_t memcached;
} worker_t;

typedef struct {
    ev_io watcher;
    struct ev_loop *loop;
    char *fifo_filename;
    char read_buffer[BUFFER_SIZE];
    char cache_buffer[BUFFER_SIZE];
} socket_worker_t;

worker_t *pool;
long int workers_count;
long int worker_contention;

socket_worker_t socket_worker;

pthread_mutex_t mutex;
pthread_cond_t cond;
int should_run = 1, url_count = 0;

int server_count = 0;
char **memcached_servers = NULL;
time_t expiration;
char key_prefix[MAX_KEY_PREFIX_LENGTH];

action_t **actions = NULL;

int create_fifo(const char *fifo);
int create_thread(pthread_t *thread, void *(*thread_routine)(void*), void *arg);
void *worker_thread(void *args);

void socket_cb(struct ev_loop *loop, struct ev_io *watcher, int events);

int write_fifo(const char *str);

void signal_handler(int signo);

void exit_(config_t *cfg, char **memcached_servers, int server_count,
        action_t **actions, int exit_code);
void usage(char *program);

int main(int argc, char**argv) {
    int error, i, verbose = 0;
    char config_filename[PATH_MAX], fifo_filename[PATH_MAX],
            pidfile_filename[PATH_MAX], include_dir[PATH_MAX] = "";
    char *str = NULL;

    /* Default configuration filename */
    strcpy(config_filename, CONFIG_FILENAME);

    /* Load options from the command line */
    int skip = 0;
    for (i = 1; i < argc; i++) {
        if (!skip) {
            if (!strcmp(argv[i], "-v")) {
                verbose = 1;
            } else if (!strcmp(argv[i], "-f")) {
                if (i + 1 < argc) {
                    strcpy(config_filename, argv[i + 1]);
                    skip = 1;
                } else {
                    usage(argv[0]);
                    exit_(NULL, memcached_servers, server_count, NULL,
                            EXIT_FAILURE);
                }
            }
        } else
            skip = 0;
    }

    /* Prepare the logger */
#ifdef DEBUG
    log_init_verbose();
#else
    if (!verbose)
        log_init_quiet();
    else
        log_init_verbose();
#endif
    log_notice("Initializing daemon");

    /* Load configuration */
    config_t cfg;
    if (load_config(&cfg, config_filename) == -1)
        exit_(&cfg, memcached_servers, server_count, NULL, EXIT_FAILURE);

    if (check_version(&cfg, (const char**) &str, QUERAL_VERSION) == -1)
        exit_(&cfg, memcached_servers, server_count, NULL, EXIT_FAILURE);

    if (read_pidfile_filename(&cfg, (const char**) &str) != -1)
        strcpy(pidfile_filename, str);
    else {
        strcpy(pidfile_filename, DEFAULT_PIDFILE_FILENAME);
        log_notice("Using default pid file: '%s'", pidfile_filename);
    }

    if (read_socket_filename(&cfg, (const char**) &str) != -1)
        strcpy(fifo_filename, str);
    else {
        strcpy(fifo_filename, DEFAULT_FIFO_FILENAME);
        log_notice("Using default fifo file: '%s'", fifo_filename);
    }

    if (read_workers_count(&cfg, &workers_count) != -1)
        workers_count = (workers_count > MAX_WORKERS) ? MAX_WORKERS
            : workers_count;
    else {
        workers_count = DEFAULT_WORKERS;
        log_notice("Using default number of workers: %d", workers_count);
    }

    if (read_worker_contention(&cfg, &worker_contention) != -1)
        worker_contention = (worker_contention > MAX_WORKER_CONTENTION_MILLIS)
        ? MAX_WORKER_CONTENTION_MILLIS : worker_contention;
    else {
        worker_contention = DEFAULT_WORKER_CONTENTION_MILLIS;
        log_notice("Using default contention: %d", worker_contention);
    }

    memcached_servers = read_memcached_servers(&cfg, &server_count);
    if (!memcached_servers)
        exit_(&cfg, memcached_servers, server_count, NULL, EXIT_FAILURE);

    if (read_memcached_expiration(&cfg, &expiration) == -1) {
        expiration = DEFAULT_MEMCACHE_EXPIRATION_SECONDS;
        log_notice("Using default memcached expiration: %d", expiration);
    }

    if (read_memcached_key_prefix(&cfg, (const char**) &str) != -1)
        strcpy(key_prefix, str);
    else {
        strcpy(key_prefix, DEFAULT_MEMCACHED_KEY_PREFIX);
        log_notice("Using default memcached key prefix: '%s'", key_prefix);
    }

    /* Load secondary configuration */
    if (read_include_dir(&cfg, (const char**) &str) != -1)
        strcpy(include_dir, str);

    if (strlen(include_dir) > 0) {
        char **include_files = list_files(include_dir,
                DEDAULT_CONF_FILE_EXTENSION);
        if (include_files) {
            actions = create_actions(include_files);
            if (!actions)
                exit_(&cfg, memcached_servers, server_count, NULL,
                    EXIT_FAILURE);
            for (i = 0; include_files[i]; i++)
                if (load_action(actions, include_files[i], i, QUERAL_VERSION)
                        == -1)
                    exit_(&cfg, memcached_servers, server_count, actions,
                        EXIT_FAILURE);
            destroy_string_list(include_files, i);
        } else
            log_warning("Failed to load configuration files from: '%s'",
                include_dir);
    }

    /* Become a daemon before installing signal handlers */
    if (daemonize(pidfile_filename) == -1)
        exit_(&cfg, memcached_servers, server_count, actions, EXIT_FAILURE);

    /* Install signal handlers */
    struct sigaction act;
    act.sa_handler = signal_handler;
    sigfillset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT, &act, NULL);
    sigaction(SIGHUP, &act, NULL);

    /* Prepare the UNIX socket */
    int socket = create_fifo(fifo_filename);
    if (socket == -1)
        exit_(&cfg, memcached_servers, server_count, actions, EXIT_FAILURE);

    /* Initialize libcurl before any threads are started */
    curl_thread_init();

    /* Prepare the pool for the worker threads */
    pool = (worker_t *) malloc(sizeof (worker_t) * workers_count);

    /* Prepare and run the worker threads */
    pthread_t thread_ids[workers_count];
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    for (i = 0; i < workers_count; i++) {
        pool[i].worker_id = i;

        error = create_thread(&thread_ids[i], worker_thread, (void *) &pool[i]);

        if (0 != error)
            log_error("Couldn't run worker thread %d, errno %d", i, error);
    }

    /* Prepare the queue */
    fifo_init(QUEUE_CAPACITY);

    /* Run the default loop in the main thread and start the socket watcher,
     * this will watch for UNIX domain socket to become readable */
    memset(socket_worker.cache_buffer, '\0', BUFFER_SIZE);

    socket_worker.loop = ev_loop_new(EVFLAG_AUTO);
    socket_worker.fifo_filename = (char *) malloc(sizeof (char)
            * (strlen(fifo_filename) + 1));
    strcpy(socket_worker.fifo_filename, fifo_filename);

    ev_io_init(&socket_worker.watcher, socket_cb, socket, EV_READ);
    socket_worker.watcher.data = &socket_worker;
    ev_io_start(socket_worker.loop, &socket_worker.watcher);

    /* Wait for events to arrive */
    ev_loop(socket_worker.loop, 0);

    /* Wait for all threads to terminate */
    for (i = 0; i < workers_count; i++) {
        error = pthread_join(thread_ids[i], NULL);
        DEBUGPRINT("Thread %d join return\n", i);
    }

    free(pool);
    fifo_destroy();
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
    destroy_actions(actions);
    destroy_string_list(memcached_servers, server_count);
    destroy_config(&cfg);

    log_notice("Terminated with status %d", EXIT_SUCCESS);

    return EXIT_SUCCESS;
}

int create_fifo(const char *fifo) {
    int socket;

    struct stat st;
    if (lstat(fifo, &st) == 0) {
        if ((st.st_mode & S_IFMT) == S_IFREG) {
            errno = EEXIST;
            log_error("File '%s' exist and is not a socket", fifo);
            return -1;
        }
    }

    unlink(fifo);
    if (mkfifo(fifo, 0660) == -1) {
        log_error("Failed to create the socket: '%s'", fifo);
        return -1;
    }

    /* We use O_RDWR instead of O_RDONLY because in Linux is more secure */
    socket = open(fifo, O_RDWR | O_NONBLOCK, 0);

    if (socket == -1) {
        log_error("Failed to open the socket: '%s'", fifo);
        return -1;
    }

    DEBUGPRINT("Ready to read data from: %s\n", fifo);

    return socket;
}

int create_thread(pthread_t *thread, void *(*thread_routine)(void*),
        void *arg) {
    sigset_t fullsigset, oldsigset;
    sigfillset(&fullsigset);

    pthread_sigmask(SIG_SETMASK, &fullsigset, &oldsigset);
    int error = pthread_create(thread, NULL, thread_routine, arg);
    pthread_sigmask(SIG_SETMASK, &oldsigset, 0);

    return error;
}

int get_command(char *command, char* job) {
    size_t job_length = strlen(job);
    memmove(job, strim(job), job_length);
    char *found = strchr(job, ' ');
    if (found) {
        size_t command_length = strlen(job) - strlen(found) + 1;
        strncpy(command, job, command_length);
        command[command_length - 1] = '\0';

        if (!strcmp(command, FETCH_COMMAND)) {
            /* Redefine the job */
            job_length = strlen(job + command_length - 1);
            memmove(job, job + command_length - 1, job_length);
            job[job_length] = '\0';

            memmove(job, strim(job), job_length);

            return FETCH;
        }

        if (!strcmp(command, CACHE_COMMAND)) {
            /* Redefine the job */
            job_length = strlen(job + command_length - 1);
            memmove(job, job + command_length - 1, job_length);
            job[job_length] = '\0';

            memmove(job, strim(job), job_length);

            return CACHE;
        }

        if (!strcmp(command, UPDATE_COMMAND)) {
            /* Redefine the job */
            job_length = strlen(job + command_length - 1);
            memmove(job, job + command_length - 1, job_length);
            job[job_length] = '\0';

            memmove(job, strim(job), job_length);

            return UPDATE;
        }
    }
    return UNDEFINED;
}

void *worker_thread(void *args) {
    int res, millis_to_sleep;
    worker_t *worker = (worker_t *) args;

    /* Initialize cURL client */
    res = curl_init(&worker->curl);
    if (res == -1) {
        log_error("Failed to create cURL at worker %d", worker->worker_id);
        return NULL;
    }

    /* Initialize the memcached client */
    worker->memcached.memc = memcached_init();
    if (memcached_servers && server_count > 0) {
        int i;
        for (i = 0; i < server_count; i++) {
            if (memcached_servers[i] && strcmp(memcached_servers[i], "")) {
                if (memcached_connect_(&worker->memcached, memcached_servers[i])
                        == -1) {
                    log_error("Failed to connect memcached %s at worker %d",
                            memcached_servers[i], worker->worker_id);
                    curl_destroy(&worker->curl);
                    memcached_destroy(&worker->memcached);
                    return NULL;
                }
                DEBUGPRINT("Worker thread %d, connected to memcached='%s'",
                        worker->worker_id, memcached_servers[i]);
            }
        }
    } else {
        log_error("Failed to get memcached server list at worker %d",
                worker->worker_id);
        curl_destroy(&worker->curl);
        memcached_destroy(&worker->memcached);
        return NULL;
    }

    DEBUGPRINT("Worker thread %d created\n", worker->worker_id);

    char job[BUFFER_SIZE], command[BUFFER_SIZE], key[MEMCACHED_MAX_KEY - 1],
            new_command[BUFFER_SIZE];
    action_t *action;
    size_t length1, length2;
    int i;

    /* Wait for events to arrive */
    for (;;) {
        pthread_mutex_lock(&mutex);
        while (url_count < 1) pthread_cond_wait(&cond, &mutex);

        if (!should_run) {
            url_count--;
            pthread_mutex_unlock(&mutex);
            break;
        }

        fifo_dequeue(job);
        url_count--;
        pthread_mutex_unlock(&mutex);

#ifdef DEBUG
        if (job)
            DEBUGPRINT("Worker %d get '%s'\n", worker->worker_id, job);
        else
            DEBUGPRINT("Worker %d, queue is empty\n", worker->worker_id);
#endif

        /* Operate with job */
        switch (get_command(command, job)) {
            case FETCH:
                worker->curl.buffer.enabled = 0;
                curl_fetch_url(&worker->curl, job);
                break;
            case CACHE:
                /* Create memcached key */
                if (create_key(key, MEMCACHED_MAX_KEY - 1, key_prefix, job)
                        == -1)
                    break;
                /* Delete the previous entry from the cache */
                memcached_delete_record(&worker->memcached, key, 0);
                /* Download the content from the URL */
                worker->curl.buffer.enabled = 1;
                res = curl_fetch_url(&worker->curl, job);
                /* Update memcached servers */
                if (res != -1) {
                    memcached_override_record(&worker->memcached, key,
                            worker->curl.buffer.memory, expiration);
                }
                break;
            case UPDATE:
                action = find_action(actions, job);
                if (action) {
                    strcpy(new_command, CACHE_COMMAND);
                    strcat(new_command, " ");
                    length1 = strlen(new_command);
                    for (i = 0; i < action->count && action->refresh_list[i];
                            i++) {
                        new_command[length1] = '\0';
                        length2 = strlen(action->refresh_list[i]);
                        if (length1 + length2 + 1 <= BUFFER_SIZE) {
                            strncat(new_command, action->refresh_list[i],
                                    length2);
                            strcat(new_command, "\n");
                            write_fifo(new_command);
                        } else
                            log_error("Maximum buffer length exceeded: '%d'",
                                BUFFER_SIZE);
                    }
                } else
                    log_notice("Failed to update a non-existent cache: '%s'",
                        job);
                break;
            case UNDEFINED:
            default:
                log_notice("Calling an undefined operation: %s", command);
                break;
        }

        /* Avoid overloading the server */
        millis_to_sleep = sleep_millis(worker_contention);
        DEBUGPRINT("Awakening after: %d milliseconds\n", millis_to_sleep);
    }

    /* Frees all resources related to the thread */
    curl_destroy(&worker->curl);
    memcached_destroy(&worker->memcached);

    return NULL;
}

int should_socket_worker_run(socket_worker_t * socket_worker) {
    pthread_mutex_lock(&mutex);
    if (!should_run) {
        /* Frees all resources related to the loop */
        ev_unloop(socket_worker->loop, EVUNLOOP_ONE);
        ev_loop_destroy(socket_worker->loop);
        unlink(socket_worker->fifo_filename);
        DEBUGPRINT("Socket unlinked: %s\n", socket_worker->fifo_filename);

        /* Signal the workers to terminate */
        int i;
        for (i = 0; i < workers_count; i++) {
            url_count++;
            pthread_cond_broadcast(&cond);
        }
    }
    pthread_mutex_unlock(&mutex);

    return should_run;
}

void socket_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
    socket_worker_t *socket_worker = (socket_worker_t *) watcher->data;

    if (!should_socket_worker_run(socket_worker)) return;

    ssize_t len = read(socket_worker->watcher.fd, socket_worker->read_buffer,
            sizeof (socket_worker->read_buffer) - 1);
    if (len == -1) {
        perror("socket read failed");
        return;
    } else if (len == 0) {
        DEBUGPRINT("Connection closed\n", 0);
        return;
    }

    socket_worker->read_buffer[len] = '\0';

    int res = 0;
    char *found = strchr(socket_worker->read_buffer, '\n');
    while (found) {
        /* Concatenates avoiding buffer overflow */
        size_t buffer_length = strlen(socket_worker->read_buffer)
                - strlen(found) + 1;
        if (strlen(socket_worker->cache_buffer) + buffer_length > BUFFER_SIZE) {
            memset(socket_worker->cache_buffer, '\0', BUFFER_SIZE);
            log_error("Parse failed, buffer overflow");
            return;
        }
        strncat(socket_worker->cache_buffer, socket_worker->read_buffer,
                buffer_length);

        /* Ignore end-of-line */
        socket_worker->cache_buffer[strlen(socket_worker->cache_buffer) - 1]
                = '\0';

        /* Wake-up the worker to handle this event */
        pthread_mutex_lock(&mutex);
        DEBUGPRINT("Parsed line: '%s'\n", socket_worker->cache_buffer);
        fifo_enqueue(socket_worker->cache_buffer, &res);
        if (res == -1) {
            memset(socket_worker->cache_buffer, '\0', BUFFER_SIZE);
            log_warning("Queue is full");
            pthread_mutex_unlock(&mutex);
            return;
        }
        url_count++;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&mutex);

        /* Restore the buffers and repeat the search */
        if (strlen(socket_worker->read_buffer) > buffer_length) {
            strcpy(socket_worker->read_buffer, ++found);
            found = strchr(socket_worker->read_buffer, '\n');
        } else {
            found = NULL;
            memset(socket_worker->read_buffer, '\0', BUFFER_SIZE);
        }
        memset(socket_worker->cache_buffer, '\0', BUFFER_SIZE);
    }
    if (strlen(socket_worker->read_buffer) > 0) {
        strcpy(socket_worker->cache_buffer, socket_worker->read_buffer);
    }
}

int write_fifo(const char *str) {
    /* Write to the socket worker */
    int fd = open(socket_worker.fifo_filename, O_WRONLY);
    if (fd < 0) {
        log_error("Failed to open socket for writing: '%s'",
                socket_worker.fifo_filename);
        return -1;
    }
    ssize_t nbyte = strlen(str);
    int res = write(fd, str, nbyte);
    close(fd);
    if (res != nbyte) {
        log_error("Failed to write on socket: '%s'",
                socket_worker.fifo_filename);
        return -1;
    }
    return 0;
}

void signal_handler(int signo) {
    switch (signo) {
        case SIGTERM:
            DEBUGPRINT("SIGTERM caught\n", 0);
        case SIGINT:
            DEBUGPRINT("SIGINT caught\n", 0);
            pthread_mutex_lock(&mutex);
            should_run = 0;
            /* Signal the socket worker to terminate */
            if (socket_worker.watcher.active)
                write_fifo("QUIT");
            pthread_mutex_unlock(&mutex);
            break;
        case SIGHUP:
            DEBUGPRINT("SIGHUP caught\n", 0);
            log_notice("SIGHUP is not supported");
            break;
        default:
            DEBUGPRINT("Unhandled signal caught\n", 0);
            break;
    }
}

void exit_(config_t *cfg, char **memcached_servers, int server_count,
        action_t **actions, int exit_code) {
    /* Actions */
    destroy_actions(actions);
    /* Server list */
    destroy_string_list(memcached_servers, server_count);
    /* Configuration */
    if (cfg)
        destroy_config(cfg);
    /* Exit */
    log_notice("Terminated with status %d", exit_code);
    exit(exit_code);
}

void usage(char *program) {
    log_info("Usage: %s [-f config] [-v]", program);
}
