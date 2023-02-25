#include "files.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>

Directory listDir(const char* path) {
    Directory cur_directory;

    DIR* dir = opendir(path);
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
    
    closedir(dir);

    return cur_directory;
}