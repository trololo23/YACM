#include <linux/limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void setToRemove(const char* path);

char* getToRemove();

void Remove(const char* cur_path);

void setToCopy(const char* path);

char* getToCopy();

void Copy(const char* cur_path);