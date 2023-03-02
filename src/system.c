#include "system.h"
#include "files.h"
#include "errhandler.h"

#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char path_to_remove[PATH_MAX] = "\0";

static char path_to_copy[PATH_MAX] = "\0";

void clearRemovePath() {
    path_to_remove[0] = '\0';
}

void clearCopyPath() {
    path_to_copy[0] = '\0';
}

void setToRemove(const char* path) {
    snprintf(path_to_remove, PATH_MAX, "%s", path);
}

char* getToRemove() {
    if (strlen(path_to_remove) > 0) {
        return path_to_remove;
    }
    return NULL;
}

char* getToCopy() {
    if (strlen(path_to_copy) > 0) {
        return path_to_copy;
    }
    return NULL;
}

void setToCopy(const char* path) {
    snprintf(path_to_copy, PATH_MAX, "%s", path);
}

void Remove() {
    if (!getToRemove()) {
        return;
    }

    char cur_file_name[PATH_MAX];
    fillFileName(path_to_remove, cur_file_name);
    char cur_file_path[PATH_MAX];
    fillFilePath(cur_file_name, cur_file_path);

    if (rename(path_to_remove, cur_file_path)) {
        setMessage("Failed to remove current file!");
        return;
    }
    clearRemovePath();
}

void Copy() {
    if (!getToCopy()) {
        return;
    }

    char cur_file_name[PATH_MAX];
    fillFileName(path_to_copy, cur_file_name);
    char cur_file_path[PATH_MAX];
    fillFilePath(cur_file_name, cur_file_path);

    int src_fd, dst_fd, res;
    unsigned char buffer[4096];

    src_fd = open(path_to_copy, O_RDONLY);
    if (src_fd < 0) {
        setMessage("Error opening file!");
        return;
    }
    dst_fd = open(cur_file_path, O_CREAT | O_WRONLY, 0644);
    if (dst_fd < 0) {
        setMessage("Error opening file!");
        return;
    }

    while ((res = read(src_fd, buffer, 4096)) != 0) {
        if (res < 0) {
            setMessage("Error reading file!");
            return;
        }

        int err = write(dst_fd, buffer, res);
        if (err == -1) {
            setMessage("Error writing file!");
            return;
        }
    }

    close(src_fd);
    close(dst_fd);

    clearCopyPath();
}
