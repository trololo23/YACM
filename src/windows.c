#include "windows.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

WINDOW* main_win;

int main_cur_ind = 0;
Directory main_cur_dir;

int MAX_X;
int MAX_Y;
int INFO_SIZE_SHIFT;

static void initColors() {
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_WHITE); /* For cur file */
    init_pair(4, COLOR_RED, COLOR_BLACK); /* For no acess files */
    init_pair(5, COLOR_CYAN, COLOR_BLACK); /* For labels */
} 

static void refreshDir() {
    if (main_cur_dir.units) {
        free(main_cur_dir.units);
    }
    main_cur_dir = listDir();
    main_cur_ind = 0;
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
    main_win = newwin(MAX_Y - 2, MAX_X / 2, 0, 0);
    getmaxyx(main_win, MAX_Y, MAX_X);
    INFO_SIZE_SHIFT = MAX_X / 2;
    box(main_win, 0, 0);
}

void displayDir() {
    int start_ind = MAX(0, main_cur_ind - (MAX_Y / 2) + 2); // Cause of borders
    for (size_t i = start_ind; i < main_cur_dir.size; ++i) {
        if (i == main_cur_ind) {
            wattron(main_win, COLOR_PAIR(3));
            mvwprintw(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf);
            wattroff(main_win, COLOR_PAIR(3));
            continue;
        } else if (main_cur_dir.units[i].type == DIRECT) {
            wattron(main_win, COLOR_PAIR(1));
            wattron(main_win, A_BOLD);
            mvwprintw(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf);
            wattroff(main_win, A_BOLD);
            wattroff(main_win, COLOR_PAIR(1));
        } else if (main_cur_dir.units[i].type == FILEE) {
            mvwprintw(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf);
        } else if (main_cur_dir.units[i].type == LINK) {
            wattron(main_win, COLOR_PAIR(2));
            mvwprintw(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf);
            wattroff(main_win, COLOR_PAIR(2));
        } else if (main_cur_dir.units[i].type == NOACCESS) {
            wattron(main_win, COLOR_PAIR(4));
            mvwprintw(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf);
            wattroff(main_win, COLOR_PAIR(4));
        } else {
            wattron(main_win, COLOR_PAIR(2));
            mvwprintw(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf);
            wattroff(main_win, COLOR_PAIR(2));  
        }
    }
}

void displayInfo() {
    wattron(main_win, COLOR_PAIR(5));
    wattron(main_win, A_BOLD);
    mvwprintw(main_win, 0, 1, "Name");
    mvwprintw(main_win, 0, INFO_SIZE_SHIFT, "Size");
    wattroff(main_win, A_BOLD);
    wattroff(main_win, COLOR_PAIR(5));

    int start_ind = MAX(0, main_cur_ind - (MAX_Y / 2) + 2);
    for (size_t i = start_ind; i < main_cur_dir.size; ++i) {
        if (main_cur_dir.units[i].type == NOACCESS) {
            continue;
        }
        wattron(main_win, A_BOLD);
        mvwprintw(main_win, i - start_ind + 1, INFO_SIZE_SHIFT,"%zu", main_cur_dir.units[i].info.size);
        wattroff(main_win, A_BOLD);
    }
}

void refreshWindows() {
    // resize later
    wclear(main_win);
    box(main_win, 0, 0);
    displayDir();
    displayInfo();
    wrefresh(main_win);
    keyboardHandle();
}

static void deleteFile() {
    if (!main_cur_ind || main_cur_dir.units[main_cur_ind].type != FILEE) {
        return;
    }
    remove_file(main_cur_dir.units[main_cur_ind].buf);
    refreshDir();
}

static void go() {
    if (main_cur_ind) {
        if (main_cur_dir.units[main_cur_ind].type == DIRECT) {
            go_dir(main_cur_dir.units[main_cur_ind].buf);
            refreshDir();
        }
    } else {
        out_dir();
        refreshDir();
    }
}

void keyboardHandle() { // Мб потом через какую-нибудь таблицу обраюотчиков это сделать
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

    } else if (ch == 'w') { // Enter doesn't work for some reasons
        go();
    } else if (ch == 'D') {
        deleteFile();
    }else if (ch == 'q') {
        endwin();
        exit(0);
    }
}
