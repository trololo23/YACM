#pragma once

#include <ncurses.h>
#include <unistd.h>

#include "files.h"

void init();
void init_windows();

void refreshWindows();
void displayDir();
void displayInfo();

void keyboardHandle();