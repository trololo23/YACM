#include "files.h"
#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>

static char cur_directory_path[PATH_MAX];

void go_dir(char* name_to_add) {
    sprintf(cur_directory_path, "%s/%s", cur_directory_path, name_to_add);
}

void out_dir() {
    char* it = strchr(cur_directory_path, '/');
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

Directory listDir() {
    Directory cur_directory;

    DIR* dir = opendir(cur_directory_path);
    if (!dir) {
        perror("Can't open dir\n");
        exit(EXIT_FAILURE);
    }

    size_t units_count = 0; 

    struct dirent *ent; 
    while ((ent = readdir(dir))) { 
        if (!strcmp(ent->d_name, ".")) {
            continue;
        }
        ++units_count;
    } 

    rewinddir(dir);

    cur_directory.units = calloc(units_count, sizeof(Unit));
    cur_directory.size = units_count;

    snprintf(cur_directory.units[0].buf, MAX_PATH, "%s", "..");

    size_t cur_ind = 1;

    while ((ent = readdir(dir))) { 
        if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) { 
            continue; 
        } else if (ent->d_type == DT_DIR) { 
            cur_directory.units[cur_ind].type = DIRECT;
        } else if (ent->d_type == DT_LNK) {
            cur_directory.units[cur_ind].type = LINK;
        } else if (ent->d_type == DT_REG) { 
            cur_directory.units[cur_ind].type = FILEE;
        } 
        snprintf(cur_directory.units[cur_ind].buf, MAX_PATH, "%s", ent->d_name);
        ++cur_ind;
    } 
    
    qsort(cur_directory.units + 1, cur_directory.size - 1, sizeof(Unit), compareUnits);

    closedir(dir);

    return cur_directory;
}