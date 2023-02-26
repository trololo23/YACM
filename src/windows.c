#include "windows.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

WINDOW* main_win;

size_t main_cur_ind = 0;
Directory main_cur_dir;

int MAX_X;
int MAX_Y;

static void initColors() {
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_WHITE); /* For cur file */
} 

static void refreshDir() {
    if (main_cur_dir.units) {
        free(main_cur_dir.units);
    }
    main_cur_dir = listDir();
}

void init() {
    initscr();
    noecho();
    curs_set(0);
    start_color();
    keypad(stdscr, TRUE);

    init_windows();
    initPath();
    refreshDir();
    initColors();
}

void init_windows() {
    getmaxyx(stdscr, MAX_Y, MAX_X);
    main_win = newwin(MAX_Y - 2, (MAX_X / 2), 0, 0);
    box(main_win, 0, 0);
}

void displayDir() {
    for (size_t i = 0; i < main_cur_dir.size; ++i) {
        if (i == main_cur_ind) {
            wattron(main_win, COLOR_PAIR(3));
            mvwprintw(main_win, i + 1, 1, main_cur_dir.units[i].buf);
            wattroff(main_win, COLOR_PAIR(3));
            continue;
        }

        if (main_cur_dir.units[i].type == DIRECT) {
            wattron(main_win, COLOR_PAIR(1));
            wattron(main_win, A_BOLD);
            mvwprintw(main_win, i + 1, 1, main_cur_dir.units[i].buf);
            wattroff(main_win, A_BOLD);
            wattroff(main_win, COLOR_PAIR(1));
        } else if (main_cur_dir.units[i].type == FILEE) {
            mvwprintw(main_win, i + 1, 1, main_cur_dir.units[i].buf);
        } else if (main_cur_dir.units[i].type == LINK) {
            wattron(main_win, COLOR_PAIR(2));
            mvwprintw(main_win, i + 1, 1, main_cur_dir.units[i].buf);
            wattroff(main_win, COLOR_PAIR(2));
        } else {
            wattron(main_win, COLOR_PAIR(2));
            mvwprintw(main_win, i + 1, 1, main_cur_dir.units[i].buf);
            wattroff(main_win, COLOR_PAIR(2));  
        }
    }
}

void refreshWindows() {
    // resize later

    displayDir();
    box(main_win, 0, 0);
    wrefresh(main_win);
    keyboardHandle();
}

void keyboardHandle() {
    int ch;
    ch = getch();
    
    if (ch == KEY_UP) {
        if (main_cur_ind > 0) {
            --main_cur_ind;
        }
    } else if (ch == KEY_DOWN) {
        if (main_cur_ind < main_cur_dir.size - 1) {
            ++main_cur_ind;
        }
    } else if (ch == KEY_RIGHT) {

    } else if (ch == KEY_LEFT) {

    } else if (ch == 'q') {
        endwin();
        exit(0);
    }
}
