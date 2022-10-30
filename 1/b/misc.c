#include "misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int t_err(char *fun_name) {
    fprintf(stderr, "[%s] %s failed\n", prog_name, fun_name);
    return -1;
}

void e_err(char *fun_name) {
    t_err(fun_name);
    fprintf(stderr, "[%s] Error: %s\n", prog_name, strerror(errno));
    exit(EXIT_FAILURE);
}