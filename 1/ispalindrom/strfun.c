/**
 * String functions module.
 * @brief Implementation of the string functions module defintions.
 * @file strfun.c
 * @author Tobias Gruber, 11912367
 * @date 12.10.2022
 */

#include "strfun.h"
#include <string.h>
#include <ctype.h>

#define ASCII_NEW_LINE 10
#define ASCII_SPACE 32

void trim(char res[], char *src) {
    int res_i = 0; /**< Current write-index of res. */
    for (int i = 0; i < strlen(src); ++i) {
        if (src[i] != ASCII_SPACE) {
            res[res_i] = src[i];
            res_i++;
        }
    }
    res[res_i] = '\0';
}

void remove_newline(char src[]) {
    if (src[strlen(src) - 1] == ASCII_NEW_LINE) {
        src[strlen(src) - 1] = '\0';
    }
}

void to_lower(char src[]) {
    for (int i = 0; i < strlen(src); i++) {
        src[i] = tolower(src[i]);
    }
}
