/**
 * Miscellaneous module.
 * @brief Implementation of the miscellaneous module definitions.
 * @file misc.c
 * @author Tobias Gruber, 11912367
 * @date 19.11.2022
 **/

#include "misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

int m_err(char *msg) {
    fprintf(stderr, "[%s] Error: %s\n", prog_name, msg);
    return -1;
}

int t_err(char *fun_name) {
    fprintf(stderr, "[%s] %s failed\n", prog_name, fun_name);
    return -1;
}

void e_err(char *fun_name) {
    t_err(fun_name);
    fprintf(stderr, "[%s] Error: %s\n", prog_name, strerror(errno));
    exit(EXIT_FAILURE);
}

void remove_newline(char src[]) {
    if (src[strlen(src) - 1] == '\n') {
        src[strlen(src) - 1] = '\0';
    }
}

int parse_int(int *dst, char *src) {
    if (src == NULL) return -1;
    char *end = NULL;
    long num;
    num = strtol(src, &end, HEX_B);
    if (*end != '\0' || num < 0) {
        errno = EINVAL;
        return m_err("Number must be a positive integer");
    }
    if (num > INT_MAX) {
        errno = EINVAL;
        return m_err("Number out of integer bounds");
    }
    *dst = (int) num;
    return 0;
}

int parse_c_int(int *dst, char src) {
    char src_str[2] = {0};
    src_str[0] = src;
    if (parse_int(dst, src_str) < 0) return t_err("parse_int");
    return 0;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

void free_arr(char *x[], int len) {
    for (int i = 0; i < len; i++) {
        if (x[i] != NULL) free(x[i]);
    }
}