#include <curses.h>
#include "src/windows.h"

int main(void) {
    init();

    while (1) {
        refreshWindows();
    }
    
    endwin();
    clearr();

    return 0;
}