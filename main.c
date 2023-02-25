#include <ncurses.h>

int main(void) {
    // initialize ncurses
    initscr();
    
    // print a message
    printw("Hello, ncurses!");

    // refresh the screen to show the message
    refresh();
    
    // wait for user input
    getch();

    // end ncurses mode
    endwin();
    
    return 0;
}