#include "hex.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int multiply(char **dst, char *x, char *y) {
    int x_int, y_int;
    if (
        parse_int(&x_int, x) == -1 ||
        parse_int(&y_int, y) == -1
    ) return t_err("parse_int");
    int prod_dec = x_int * y_int;
    int len = strlen(x) * 2;
    *dst = (char *) malloc(sizeof(char) * (len + 1));
    if (*dst == NULL) return t_err("malloc");
    sprintf(*dst, "%x", prod_dec);
    fill_zeroes(dst, len, false);
    return 0;
}

int is_hex(char *str) {
    int valid_symbols = str[strspn(str, "0123456789abcdefABCDEF")];
    return (strlen(str) != 0 && valid_symbols == 0) ? 0 : -1;
}

void half_str(char *dst, char *src, int half, int half_length) {
    char *start = src;
    if (half == 1) start += half_length;
    strncpy(dst, start, half_length);
    dst[half_length] = '\0';
}

int fill_zeroes(char **str, int min_len, bool pow_of_two) {
    char old_str[strlen(*str) + 1];
    strcpy(old_str, *str);
    int new_len = 1;
    if (pow_of_two == true) {
        while (new_len < min_len) new_len *= 2;
    } else {
        new_len = min_len;
    }
    int diff = new_len - strlen(old_str);
    if (diff <= 0) return 0;
    free(*str);
    *str = (char *) calloc(new_len + 1, sizeof(char));
    if (str == NULL) return t_err("calloc");
    memset(*str, '0', diff);
    strcat(*str, old_str);
    return 0;
}

int shift_left(char **str, int n) {
    int len = strlen(*str);
    *str = (char *) realloc(*str, sizeof(char) * (len + n + 1));
    if (str == NULL) return t_err("realloc");
    for (int i = 0; i < n; i++) {
        (*str)[len + i] = '0';
    }
    (*str)[len + n] = '\0';
    return 0;
}

int add(char **res, char **x, char **y) {
    int len = max(strlen(*x), strlen(*y));
    if (fill_zeroes(x, len, false) == -1) return t_err("fill_zeroes");
    if (fill_zeroes(y, len, false) == -1) return t_err("fill_zeroes");
    char sum[len + 1];
    memset(sum, '0', len);
    int overflow = 0;
    for (int i = len - 1; i >= 0; i--) {
        int x_dec, y_dec;
        if (parse_c_int(&x_dec, (*x)[i]) == -1) return t_err("parse_c_int");
        if (parse_c_int(&y_dec, (*y)[i]) == -1) return t_err("parse_c_int");
        int sum_dec = x_dec + y_dec + overflow;
        overflow = 0;
        if (sum_dec > 15) {
            sum_dec -= 16;
            overflow = 1;
        }
        char sum_hex[2];
        sprintf(sum_hex, "%x", sum_dec);
        sum[i] = sum_hex[0];
    }
    *res = (char *) realloc(*res, sizeof(char) * (len + 1 + overflow));
    if (*res == NULL) return t_err("realloc");
    (*res)[0] = '1';
    strncpy((*res) + overflow, sum, len + overflow);
    (*res)[len + overflow] = '\0';
    return 0;
}