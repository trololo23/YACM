#pragma once

#include <linux/limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void setToRemove(const char* path);

char* getToRemove();

void Remove(); /* Overwrites the file, if there is file woth the same name */

void setToCopy(const char* path);

char* getToCopy();

void Copy();

void clearRemovePath();

void clearCopyPath();