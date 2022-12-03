/**
 * Miscellaneous module
 * @brief Implementation of the miscellaneous module definitions.
 * @file misc.c
 * @author Tobias Gruber, 11912367
 * @date 30.10.2022
 **/

#include "misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#define ASCII_NEW_LINE 10

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
    if (src[strlen(src) - 1] == ASCII_NEW_LINE) {
        src[strlen(src) - 1] = '\0';
    }
}

int parse_int(int *dst, char *src) {
    if (src == NULL) return -1;
    char *end = NULL;
    long num;
    num = strtol(src, &end, 10);
    if (*end != '\0' || num < 0) return m_err("Number must be a positive integer");
    if (num > INT_MAX) return m_err("Number out of integer bounds");
    *dst = (int) num;
    return 0;
}

int is_hex(char *str) {
    int valid_symbols = str[strspn(str, "0123456789abcdefABCDEF")];
    return (strlen(str) != 0 && valid_symbols == 0) ? 0 : -1;
}

int fill_zeroes(char **str, int min_len) {
    char old_str[strlen(*str) + 1];
    int new_len = 1;
    strcpy(old_str, *str);
    while (new_len < min_len) new_len *= 2;
    int diff = new_len - strlen(old_str);
    if (diff == 0) return 0;
    // TODO: use realloc without leak
    free(*str);
    *str = (char *) calloc(new_len + 1, sizeof(char));
    if (str == NULL) return t_err("calloc");
    memset(*str, '0', diff);
    strcat(*str, old_str);
    return 0;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

void multiply(long *dst, char *x, char *y) {
    *dst = strtol(x, NULL, 16) * strtol(y, NULL, 16);
}

void half_str(char *dst, char *src, int half, int half_length) {
    char *start = src;
    if (half == 1) start += half_length;
    strcpy(dst, start);
    if (half == 0) dst[half_length] = '\0';
}