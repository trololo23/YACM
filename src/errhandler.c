#include "errhandler.h"
#include <linux/limits.h>

static char message[PATH_MAX] = "\0";

const char* getMessage() {
    return message;
}

void setMessage(const char *mess) {
    snprintf(message, PATH_MAX, "%s", mess);
}