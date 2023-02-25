#include "windows.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

WINDOW* main_win;
int MAX_X;
int MAX_Y;

static void initColors() {
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
} 

void init() {
    initscr();
    noecho();
    curs_set(0);
    start_color();

    initColors();
}

void init_windows() {
    getmaxyx(stdscr, MAX_Y, MAX_X);
    main_win = newwin(MAX_Y - 2, (MAX_X / 2), 0, 0);
    box(main_win, 0, 0);
}

void displayDir() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd");
        exit(EXIT_FAILURE);
    }

    Directory cur_dir = listDir(cwd);
    for (size_t i = 0; i < cur_dir.size; ++i) {
        if (cur_dir.units[i].type == DIRECT) {
            wattron(main_win, COLOR_PAIR(1));
            wattron(main_win, A_BOLD);
            mvwprintw(main_win, i + 1, 1, cur_dir.units[i].buf);
            wattroff(main_win, A_BOLD);
            wattroff(main_win, COLOR_PAIR(1));
        } else if (cur_dir.units[i].type == FILEE) {
            mvwprintw(main_win, i + 1, 1, cur_dir.units[i].buf);
        } else if (cur_dir.units[i].type == LINK) {
            wattron(main_win, COLOR_PAIR(2));
            mvwprintw(main_win, i + 1, 1, cur_dir.units[i].buf);
            wattroff(main_win, COLOR_PAIR(2));
        } else {
            mvwprintw(main_win, i + 1, 1, cur_dir.units[i].buf);
        }
    }
}

void refreshWindows() {
    // resize later
    displayDir();

    wrefresh(main_win);
}
