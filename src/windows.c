#include "windows.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

WINDOW* main_win;
WINDOW* path_win;

int main_cur_ind = 0;
Directory main_cur_dir;

int MAX_X;
int MAX_Y;

int MAIN_X;
int MAIN_Y;

int INFO_SIZE_SHIFT;
int INFO_PERM_SHIFT;

int show_hidden_files = 0;

#define COLOR_ORANGE 8

static void initColors() {
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_CYAN, COLOR_WHITE); /* For cur file */
    init_pair(4, COLOR_RED, COLOR_BLACK); /* For no acess files */
    init_pair(5, COLOR_CYAN, COLOR_BLACK); /* For labels */

    init_color(COLOR_ORANGE, 1000, 500, 0);
    init_pair(6, COLOR_ORANGE, COLOR_BLACK); /* For path */
    init_pair(7, COLOR_YELLOW, COLOR_BLACK); /* For hidden files */
} 

static void refreshDir() {
    if (main_cur_dir.units) {
        free(main_cur_dir.units);
    }
    main_cur_dir = listDir(show_hidden_files);
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
    { // main window
        getmaxyx(stdscr, MAX_Y, MAX_X); 
        main_win = newwin(MAX_Y - 2, MAX_X / 2, 0, 0);
        getmaxyx(main_win, MAIN_Y, MAIN_X);
        INFO_SIZE_SHIFT = MAIN_X / 2;
        INFO_PERM_SHIFT = MAIN_X - 12;
        box(main_win, 0, 0);
    }

    { // lower info window
        path_win = newwin(1, MAIN_X, MAX_Y - 2, 0);
    }
}

void displayDir() {
    int start_ind = MAX(0, main_cur_ind - (MAIN_Y / 2) + 2); // Cause of borders
    for (size_t i = start_ind; i < main_cur_dir.size; ++i) {
        if (i - start_ind + 1 == MAIN_Y - 1) {
            break;
        }

        if (i == main_cur_ind) {
            wattron(main_win, COLOR_PAIR(3));
            mvwprintw(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf);
            wattroff(main_win, COLOR_PAIR(3));
        } else if (main_cur_dir.units[i].info.rights & R_ISHIDE) { 
            wattron(main_win, COLOR_PAIR(7));
            mvwprintw(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf);
            wattroff(main_win, COLOR_PAIR(7));
        } else if (!(main_cur_dir.units[i].info.rights & R_ISREAD)) { 
            wattron(main_win, COLOR_PAIR(4));
            mvwprintw(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf);
            wattroff(main_win, COLOR_PAIR(4));
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
    mvwprintw(main_win, 0, INFO_PERM_SHIFT, "Perms");
    wattroff(main_win, COLOR_PAIR(5));

    wattron(path_win, COLOR_PAIR(6));
    mvwprintw(path_win, 0, 0, getPath());
    wattroff(path_win, A_BOLD);
    wattroff(path_win, COLOR_PAIR(6));

    int start_ind = MAX(0, main_cur_ind - (MAIN_Y / 2) + 2);
    for (size_t i = start_ind; i < main_cur_dir.size; ++i) {
        if (i - start_ind + 1 == MAIN_Y - 1) {
            break;
        }
        if (i == 0 || !(main_cur_dir.units[i].info.rights & R_ISREAD) || (main_cur_dir.units[i].info.rights & R_ISHIDE && !show_hidden_files)) {
            continue;
        }
        wattron(main_win, A_BOLD);
        mvwprintw(main_win, i - start_ind + 1, INFO_SIZE_SHIFT,"%zu", main_cur_dir.units[i].info.size);
        mvwprintw(main_win, i - start_ind + 1, INFO_PERM_SHIFT, main_cur_dir.units[i].info.perms);
        wattroff(main_win, A_BOLD);
    }
}

void refreshWindows() {
    // resize later
    wclear(main_win);
    wclear(path_win);

    box(main_win, 0, 0);
    displayDir();
    displayInfo();

    wrefresh(main_win);
    wrefresh(path_win);

    keyboardHandle();
}

///////////////////////////////////////////////////////////////////// 

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

static void change_hide_mode() {
    show_hidden_files = 1 - show_hidden_files;
    refreshDir();
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
    } else if (ch == 'H') {
        change_hide_mode();
    }else if (ch == 'q') {
        endwin();
        exit(0);
    }
}
