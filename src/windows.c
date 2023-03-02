#include "windows.h"
#include "errhandler.h"

#include <curses.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include "system.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

WINDOW* main_win;
WINDOW* co_win;
WINDOW* path_win;
WINDOW* mess_win;

int main_cur_ind = 0;
Directory main_cur_dir;

int MAX_X;
int MAX_Y;

int MAIN_X;
int MAIN_Y;

int INFO_SIZE_SHIFT;
int INFO_PERM_SHIFT;
int INFO_PATH_SHIFT;

int show_hidden_files = 0;

#define SET_COLOR_NO_BOLD(win, y, x, ch, color_ind) \
    wattron(win, COLOR_PAIR(color_ind));            \
    mvwprintw(win, y, x, ch);                       \
    wattroff(win, COLOR_PAIR(color_ind));

#define SET_COLOR_BOLD(win, y, x, ch, color_ind) \
    wattron(win, A_BOLD);                        \
    SET_COLOR_NO_BOLD(win, y, x, ch, color_ind)  \
    wattroff(win, A_BOLD);

#define COLOR_ORANGE 8

static void initColors() {
    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK); /* Don't remember */
    init_pair(3, COLOR_CYAN, COLOR_WHITE);   /* For cur file */
    init_pair(4, COLOR_RED, COLOR_BLACK);    /* For no acess files */
    init_pair(5, COLOR_CYAN, COLOR_BLACK);   /* For labels */

    init_color(COLOR_ORANGE, 1000, 500, 0);
    init_pair(6, COLOR_ORANGE, COLOR_BLACK);  /* For path */
    init_pair(7, COLOR_YELLOW, COLOR_BLACK);  /* For hidden files */
    init_pair(8, COLOR_GREEN, COLOR_BLACK);   /* Other files */
    init_pair(9, COLOR_ORANGE, COLOR_BLUE);   /* Files to remove */
    init_pair(10, COLOR_ORANGE, COLOR_GREEN); /* Files to copy */
}

static void refreshDir() { /* Updates loaded directory info, allowes to not go over directory each frame */
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
    {  // main window
        getmaxyx(stdscr, MAX_Y, MAX_X);
        main_win = newwin(MAX_Y - 2, MAX_X / 2, 0, 0);
        getmaxyx(main_win, MAIN_Y, MAIN_X);
        INFO_SIZE_SHIFT = MAIN_X / 2;
        INFO_PERM_SHIFT = MAIN_X - 12;
        box(main_win, 0, 0);
    }

    {  // co window
        co_win = newwin(MAIN_Y, MAIN_X, 0, MAIN_X);
        box(co_win, 0, 0);
    }

    {  // lower info window
        path_win = newwin(1, MAIN_X, MAX_Y - 2, 0);
        INFO_PATH_SHIFT = 10;
    }

    {  // mess win
        mess_win = newwin(1, MAIN_X, MAX_Y - 2, MAIN_X);
    }
}

void displayDir() {
    int start_ind = MAX(0, main_cur_ind - (MAIN_Y / 2) + 2);  // Cause of borders
    for (size_t i = start_ind; i < main_cur_dir.size; ++i) {
        if (i - start_ind + 1 == MAIN_Y - 1) {
            break;
        } else if (i == main_cur_ind) {
            SET_COLOR_NO_BOLD(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf, 3);
        } else if (main_cur_dir.units[i].info.rights & R_ISCUT) {
            SET_COLOR_BOLD(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf, 9);
        } else if (main_cur_dir.units[i].info.rights & R_ISCOPY) {
            SET_COLOR_BOLD(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf, 10);
        } else if (main_cur_dir.units[i].info.rights & R_ISHIDE) {
            SET_COLOR_NO_BOLD(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf, 7);
        } else if (!(main_cur_dir.units[i].info.rights & R_ISREAD)) {
            SET_COLOR_NO_BOLD(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf, 4);
        } else if (main_cur_dir.units[i].type == DIRECT) {
            SET_COLOR_BOLD(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf, 1);
        } else if (main_cur_dir.units[i].type == FILEE) {
            mvwprintw(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf);
        } else if (main_cur_dir.units[i].type == LINK) {
            SET_COLOR_NO_BOLD(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf, 2);
        } else {
            SET_COLOR_NO_BOLD(main_win, i - start_ind + 1, 1, main_cur_dir.units[i].buf, 8);
        }
    }
}

void displayInfo() {
    /* Set Up Info */
    SET_COLOR_BOLD(main_win, 0, 1, "Name", 5);
    SET_COLOR_BOLD(main_win, 0, INFO_SIZE_SHIFT, "Size", 5);
    SET_COLOR_BOLD(main_win, 0, INFO_PERM_SHIFT, "Perms", 5);
    SET_COLOR_BOLD(path_win, 0, INFO_PATH_SHIFT, getPath(), 6);

    /* Set directory info */
    char cur_file_str[20];
    sprintf(cur_file_str, "(%d/%d)", main_cur_ind + 1, main_cur_dir.size);
    SET_COLOR_BOLD(path_win, 0, 1, cur_file_str, 6);

    /* Set message info */
    const char* mess = getMessage();
    SET_COLOR_BOLD(mess_win, 0, 1, mess, 6);

    int start_ind = MAX(0, main_cur_ind - (MAIN_Y / 2) + 2);
    for (size_t i = start_ind; i < main_cur_dir.size; ++i) {
        if (i - start_ind + 1 == MAIN_Y - 1) {
            break;
        }
        if (i == 0 || !(main_cur_dir.units[i].info.rights & R_ISREAD) ||
            (main_cur_dir.units[i].info.rights & R_ISHIDE && !show_hidden_files)) {
            continue;
        }
        wattron(main_win, A_BOLD);
        mvwprintw(main_win, i - start_ind + 1, INFO_SIZE_SHIFT, "%zu", main_cur_dir.units[i].info.size);
        mvwprintw(main_win, i - start_ind + 1, INFO_PERM_SHIFT, main_cur_dir.units[i].info.perms);
        wattroff(main_win, A_BOLD);
    }
}

void refreshWindows() {
    // resize later
    werase(main_win);
    werase(path_win);
    werase(co_win);
    werase(mess_win);

    box(main_win, 0, 0);
    box(co_win, 0, 0);
    displayDir();
    displayInfo();

    wrefresh(main_win);
    wrefresh(path_win);
    wrefresh(co_win);
    wrefresh(mess_win);

    keyboardHandle();
}

///////////////////////////  Key Handlers  //////////////////////////////////////////

static void ctrlD() {
    if (!main_cur_ind || main_cur_dir.units[main_cur_ind].type != FILEE) {
        return;
    }
    remove_file(main_cur_dir.units[main_cur_ind].buf);
    refreshDir();
}

static void enter() {
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

static void ctrlX() {
    if (!(main_cur_dir.units[main_cur_ind].info.rights & R_ISREAD)) {
        setMessage("No permissions to remove file!");
        return;
    }
    clearCopyPath();
    char path_to_file[PATH_MAX];
    fillFilePath(main_cur_dir.units[main_cur_ind].buf, path_to_file);
    setToRemove(path_to_file);
    refreshDir();
}

static void ctrlC() {
    if (!(main_cur_dir.units[main_cur_ind].info.rights & R_ISREAD)) {
        setMessage("No permissions to copy file!");
        return;
    }
    clearRemovePath();
    char path_to_file[PATH_MAX];
    fillFilePath(main_cur_dir.units[main_cur_ind].buf, path_to_file);
    setToCopy(path_to_file);
    refreshDir();
}

static void ctrlV() {
    if (getToRemove()) {
        Remove();
    } else {
        Copy();
    }
    refreshDir();
}

void keyboardHandle() {  // Мб потом через какую-нибудь таблицу обраюотчиков это сделать
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

    } else if (ch == 'w') {
        enter();
    } else if (ch == 'D') {
        ctrlD();
    } else if (ch == 'H') {
        change_hide_mode();
    } else if (ch == 'X') {
        ctrlX();
    } else if (ch == 'V') {
        ctrlV();
    } else if (ch == 'C') {
        ctrlC();
    } else if (ch == 'q') {
        endwin();
        exit(0);
    }
}
