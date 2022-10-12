#include "strfun.h"
#include <string.h>
#include <ctype.h>

void trim(char res[], char *src) {
    int res_i = 0;
    for (int i = 0; i < strlen(src); ++i) {
        if (src[i] != 32) {
            res[res_i] = src[i];
            res_i++;
        }
    }
    res[res_i] = '\0';
}

void remove_newline(char src[]) {
    if (src[strlen(src) - 1] == 10) {
        src[strlen(src) - 1] = '\0';
    }
}

void to_lower(char src[]) {
    for (int i = 0; i < strlen(src); i++) {
        src[i] = tolower(src[i]);
    }
}
