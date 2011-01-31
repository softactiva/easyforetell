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

#include "daemon_utils.h"

int lockfile(int fd) {
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;
    return (fcntl(fd, F_SETLK, &fl));
}

int already_running(const char *lock_file) {
    int fd = open(lock_file, O_RDWR | O_CREAT,
            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
        log_error("Failed to open the lock file: '%s'", lock_file);
        return 1;
    }
    if (lockfile(fd) < 0) {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            return 1;
        }
        log_error("Failed to lock the daemon");
        return 1;
    }
    char buf[16];
    int unused __attribute__((unused)) = ftruncate(fd, 0);
    sprintf(buf, "%ld", (long) getpid());
    ssize_t unused1 __attribute__((unused)) = write(fd, buf, strlen(buf) + 1);
    return 0;
}

int daemonize(const char *pidfile) {
    const char *lock_file = pidfile;

    /* Clear file creation mask */
    umask(0);

    /* Get maximum number of file descriptors */
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        log_error("Failed to get file limit");
        return -1;
    }

    /* Keep on foreground (just for application profilers!) */
#ifndef QUERAL_PROFILE

    /* Become a session leader to lose controlling TTY */
    pid_t pid;
    if ((pid = fork()) < 0) {
        log_error("Failed to fork a new process");
        return -1;
    } else if (pid != 0) {
        /* Terminate parent */
        exit(EXIT_SUCCESS);
    }

    /* Create a new Session ID (SID) for the child process */
    pid_t sid;
    if ((sid = setsid()) < 0) {
        log_error("Failed to create a new SID for the child process");
        return -1;
    }

#endif

    /* Ensure future opens won't allocate controlling TTYs */
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        log_error("Failed to ignore SIGHUP");
        return -1;
    }

    /* Change the current working directory to the to some place that is
     * guaranteed to always be there so we won't prevent file systems from
     * being unmounted */
    if (chdir(DAEMON_WORKING_DIR) < 0) {
        log_error("Failed to change the working directory to: '%s'",
                DAEMON_WORKING_DIR);
        return -1;
    }

    /* Close all open file descriptors */
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;

    unsigned int i;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);

    /* Close out the standard file descriptors. After this point, no messages
     * will be sent to the console */
    int fd0 = open(DAEMON_STDIN_FILE, O_RDONLY);
    int fd1 = open(DAEMON_STDOUT_FILE,
            O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    int fd2 = open(DAEMON_STDERR_FILE,
            O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

    if (fd0 != STDIN_FILENO || fd1 != STDOUT_FILENO || fd2 != STDERR_FILENO) {
        /* Unexpected file descriptors */
        log_error("Standard file descriptors were not opened as expected");
        return -1;
    }

    /* Make sure only one copy of the daemon is running */
    if (already_running(lock_file)) {
        log_error("Failed to run or daemon already running");
        /* Nothing to do: we have no console here */
        return -1;
    }

    /* We are up and running */
    log_info("Daemon started");

    return 1;
}
