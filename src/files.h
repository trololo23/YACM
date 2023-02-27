#include <ncurses.h>
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "system.h"

enum { DIRECT = 0, FILEE = 1, LINK = 2 };

#define MAX_PATH 50

enum { R_ISHIDE = 0b1, R_ISREAD = 0b10, R_ISCUT = 0b100 };

typedef struct Info {
    char perms[10];
    size_t size;
    int8_t rights;
} Info;

typedef struct Unit {
    Info info;
    char buf[MAX_PATH];
    int type;     
} Unit;

typedef struct Directory {
    int size;
    Unit* units;
} Directory;

Directory listDir(int mode);

void initPath();

void go_dir(char* name_to_add);

void out_dir();

void remove_file(const char *file_name);

const char* getPath();

void fillFilePath(const char* file_name, char* buf);