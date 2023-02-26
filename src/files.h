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

enum { DIRECT = 0, FILEE = 1, LINK = 2};

#define MAX_PATH 50

typedef struct Unit {
    char buf[MAX_PATH];
    int type;
} Unit;

typedef struct Directory {
    int size;
    Unit* units;
} Directory;

// returns number of files/dirs 
Directory listDir();

void initPath();

void go_dir(char* name_to_add);

void out_dir();