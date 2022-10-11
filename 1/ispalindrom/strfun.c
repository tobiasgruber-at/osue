#include "strfun.h"
#include <string.h>

void trim(char res[], char line[]) {
    /*int res_i = 0;
    for (int i = 0; i < strlen(line); ++i) {
        if (line[i] != 32) {
            res[res_i] = line[i];
            res_i++;
        }
    }*/

    /*//printf("%lu\n", sizeof(char) * (strlen(*line) + 1));
    char *res_p = (char *) malloc(sizeof(char) * (strlen(*line) + 1));
    printf("1");
    char *cur_p = res_p;
    printf("2");
    for (int i = 0; i < strlen(*line); ++i) {
        if ((*line)[i] != 32) {
            *cur_p = (*line)[i];
            cur_p++;
        }
    }
    *cur_p = '\0';
    *line = res_p;
    printf("1: %s", res_p);*/
}

char *remove_newline(char line[]) {
    if (line[strlen(line) - 1] == 10) {
        line[strlen(line) - 1] = '\0';
    }
    return line;
}
