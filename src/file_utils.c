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

#include "file_utils.h"

int is_file(const char *filename) {
    int fd, res = 0;
    struct stat st;
    if ((fd = open(filename, O_RDONLY)) < 0) {
        log_notice("Failed to open file '%s': %s", filename, strerror(errno));
        return 0;
    }
    fstat(fd, &st);
    if (S_ISREG(st.st_mode))
        res = 1;
    close(fd);
    return res;
}

char **list_files(const char *dir_pathname, const char *file_extension) {
    DIR *dirp;
    struct dirent *direntp;
    char filename[PATH_MAX];
    int file_length, dir_length, ext_length;

    if (!file_extension) {
        log_error("Invalid file extension");
        return NULL;
    }

    if ((dirp = opendir(dir_pathname)) == NULL) {
        log_error("Failed to open directory '%s': %s", dir_pathname,
                strerror(errno));
        return NULL;
    }

    char **file_list = init_string_list(MAX_NUM_FILES);
    if (!file_list) {
        log_error("Failed to allocate memory for listing files");
        closedir(dirp);
        return NULL;
    }

    strcpy(filename, dir_pathname);
    if (strcmp(dir_pathname + (strlen(dir_pathname) - 1), UNIX_FILE_SEPARATOR))
        strcat(filename, UNIX_FILE_SEPARATOR);
    dir_length = strlen(filename);
    ext_length = strlen(file_extension);

    int i = 0;
    while (i < MAX_NUM_FILES && (direntp = readdir(dirp)) != NULL) {
        file_length = strlen(direntp->d_name);
        if (file_length > ext_length
                && !strcmp((direntp->d_name + file_length - ext_length),
                file_extension))
            strcpy(filename + dir_length, direntp->d_name);
        else
            continue;
        if (is_file(filename)) {
            if ((file_list[i] = dup_string(filename)))
                i++;
            else {
                log_error("Failed to allocate memory for listing files");
                destroy_string_list(file_list, i);
                closedir(dirp);
                return NULL;
            }
        }
    }

    if (i < MAX_NUM_FILES)
        file_list[i] = NULL;
    else
        log_warning("Maximum number of files exceeded on '%s': %d",
            dir_pathname, MAX_NUM_FILES);

    closedir(dirp);
    return file_list;
}
