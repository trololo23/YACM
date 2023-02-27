#include "system.h"
#include <linux/limits.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static char path_to_remove[PATH_MAX] = "\0";

static char path_to_copy[PATH_MAX] = "\0";

void setToRemove(const char *path) {
    snprintf(path_to_remove, PATH_MAX, "%s", path);
}

char* getToRemove() {
    if (strlen(path_to_remove) > 0) {
        return path_to_remove;
        exit(0);
    }
    return NULL;
}

void setToCopy(const char *path) {
    snprintf(path_to_copy, PATH_MAX, "%s", path);
}

void Remove(const char* cur_path) {
    if (!strlen(path_to_remove)) {
        return;
    }
    int result = rename(path_to_remove, cur_path);
    if (result != 0) {
        perror("Failed to remove current file");
    }
    path_to_remove[0] = '\0';
}

