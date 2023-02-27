#include "files.h"
#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static char cur_directory_path[PATH_MAX];

static const char* perms_to_str(char* buf, int perms) {
    const char* fmt[] = {"---", "--x", "-w-", "-wx", "r--", "r-x", "rw-", "rwx"};
    sprintf(buf, "%s%s%s", fmt[(perms & 0700) >> 6], fmt[(perms & 0070) >> 3], fmt[perms & 0007]);

    if ((perms & S_ISUID) && ((perms & S_IXGRP) || (perms & S_IXOTH))) {
        buf[2] = 's';
    }

    if ((perms & S_ISGID) && (perms & S_IXOTH)) {
        buf[5] = 's';
    }

    if ((perms & S_ISVTX) && (perms & S_IXOTH) && (perms & S_IWOTH)) {
        buf[8] = 't';
    }

    buf[9] = '\0';

    return buf;
}

void go_dir(char* name_to_add) {
    sprintf(cur_directory_path, "%s/%s", cur_directory_path, name_to_add);
}

void out_dir() {
    char* it = strrchr(cur_directory_path, '/');
    *it = '\0';
}

void initPath() {
    if (getcwd(cur_directory_path, sizeof(cur_directory_path)) == NULL) {
        perror("Error with getting current path\n");
        exit(EXIT_FAILURE);
    }
}

static int compareUnits(const void* first, const void* second) {
    const Unit* first_p = (const Unit*)(first);
    const Unit* second_p = (const Unit*)(second);
    return strcmp(first_p->buf, second_p->buf);
}

static void fillInfo(size_t ind, const Directory* dir) {
    char abs_path[PATH_MAX]; /* Set default rights */
    fillFilePath(dir->units[ind].buf, abs_path);
    dir->units[ind].info.rights = 0;

    if (dir->units[ind].buf[0] == '.') { /* Set hiddens */
        dir->units[ind].info.rights |= R_ISHIDE;
    }

    char* file_to_remove_path = getToRemove(); /* Set to remove */
    if (file_to_remove_path && !strcmp(abs_path, file_to_remove_path)) {
        dir->units[ind].info.rights |= R_ISCUT;
    }

    if (access(abs_path, R_OK) == -1) { /* Set no access */
        return;
    }
    dir->units[ind].info.rights |= R_ISREAD;

    struct stat unit_stat; /* Fill other info to display later */
    if (stat(abs_path, &unit_stat) == 0) {
        dir->units[ind].info.size = unit_stat.st_size;
        perms_to_str(dir->units[ind].info.perms, unit_stat.st_mode);
    }
}

Directory listDir(int hide_mode) {
    Directory cur_directory;

    DIR* dir = opendir(cur_directory_path);
    if (!dir) {
        perror("Can't open dir\n");
        exit(EXIT_FAILURE);
    }

    size_t units_count = 0;

    struct dirent* ent;
    while ((ent = readdir(dir))) {
        if (!strcmp(ent->d_name, ".")) {
            continue;
        }
        if (strcmp(ent->d_name, "..") && ent->d_name[0] == '.' && !hide_mode) {  // Don't look at hidden files
            continue;
        }

        ++units_count;
    }

    rewinddir(dir);

    cur_directory.units = calloc(units_count, sizeof(Unit));
    cur_directory.size = units_count;

    snprintf(cur_directory.units[0].buf, MAX_PATH, "%s", "..");
    cur_directory.units[0].info.rights = 0b10;

    size_t cur_ind = 1;

    while ((ent = readdir(dir))) {
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) {
            continue;
        } else if (ent->d_name[0] == '.' && !hide_mode) {
            continue;
        } else if (ent->d_type == DT_DIR) {
            cur_directory.units[cur_ind].type = DIRECT;
        } else if (ent->d_type == DT_LNK) {
            cur_directory.units[cur_ind].type = LINK;
        } else if (ent->d_type == DT_REG) {
            cur_directory.units[cur_ind].type = FILEE;
        }

        snprintf(cur_directory.units[cur_ind].buf, MAX_PATH, "%s", ent->d_name);
        fillInfo(cur_ind, &cur_directory);
        ++cur_ind;
    }

    qsort(cur_directory.units + 1, cur_directory.size - 1, sizeof(Unit), compareUnits);

    closedir(dir);

    return cur_directory;
}

void remove_file(const char* file_name) {
    char path_to_file[PATH_MAX];
    fillFilePath(file_name, path_to_file);
    if (access(path_to_file, W_OK) == 0) {
        remove(path_to_file);
    } else {
        // some error handling, think later
    }
}

const char* getPath() {
    return cur_directory_path;
}

void fillFilePath(const char *file_name, char *buf) {
    snprintf(buf, PATH_MAX, "%s/%s", cur_directory_path, file_name);
}