#include "open.h"
#include "../errhandler.h"

#include <curses.h>

#include <linux/limits.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static const char* get_ext(const char* file_path) {
    const char* it = strrchr(file_path, '.');
    return it + 1;
}

static void run_ide(const char* ide_path, const char* name, const char* file_path) {
    int pid = fork();
    if (pid == -1) {
        setMessage("System error, fork failed!");
        return;
    } else if (pid == 0) {
        execl(ide_path, name, file_path, (char*)NULL);
        setMessage("System error, fork failed!");
        return;
    } else {
        endwin();
        waitpid(pid, NULL, 0);
        initscr();
    }
}

int open_file(const char* path) {
    const char* ext = get_ext(path);
    if (!strcmp("txt", ext)) {
        run_ide("/usr/bin/vim", "vim", path);
    } else if (!strcmp("cpp", ext) || !strcmp("c", ext)) {
        run_ide("/snap/bin/code", "code", path);
    }

    return 0;
}
