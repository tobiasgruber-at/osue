#include "misc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *prog_name;

void usage() {
    fprintf(stderr, "Usage: %s\n", prog_name);
    exit(EXIT_FAILURE);
}

void fill_zeroes(char **str, int min_len) {
    char old_str[strlen(*str) + 1];
    int new_len = 1;
    strcpy(old_str, *str);
    while (new_len < min_len) new_len *= 2;
    int diff = new_len - strlen(old_str);
    if (diff == 0) return;
    *str = (char *) malloc(sizeof(char) * new_len);
    for (int i = 0; i < diff; i++) (*str)[i] = '0';
    strcat(*str, old_str);
}

// compare two hexadecimal numbers
int cmp_hex(char *a, char *b) {
    int len_a = strlen(a);
    int len_b = strlen(b);
    if (len_a > len_b) return 1;
    if (len_a < len_b) return 0;
    for (int i = 0; i < len_a; i++) {
        if (a[i] > b[i]) return 1;
        if (a[i] < b[i]) return 0;
    }
    return 0;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int handle_options(char *rands[]) {
    char *line = NULL; /**< String of the current line. */
    size_t len = 0; /**< Length of the current line. */
    int line_c = 0;
    while ((getline(&line, &len, stdin)) > 0) {
        if (line_c >= 2) return m_err("More than two hexadecimal numbers provided");
        remove_newline(line);
        if (is_hex(line) == -1) {
            m_err("Input must be a hexadecimal number");
            return t_err("is_hex");
        }
        rands[line_c] = (char *) malloc(sizeof(char) * (strlen(line) + 1));
        strcpy(rands[line_c], line);
        ++line_c;
    }
    if (line_c < 2) return m_err("Less than two hexadecimal numbers provided");
    free(line);
    int max_length = max(strlen(rands[0]), strlen(rands[1]));
    for (int i = 0; i < 2; i++) fill_zeroes(&rands[i], max_length);
    return 0;
}

void free_rands(char *rands[2]) {
    for (int i = 0; i < 2; i++) free(rands[i]);
}

int main(int argc, char **argv) {
    prog_name = argv[0];
    if (argc > 1) usage();
    char *rands[2];
    if (handle_options(rands) == -1) {
        free_rands(rands);
        e_err("handle_options");
    }
    printf("%s\n", rands[0]);
    printf("%s\n", rands[1]);
    free_rands(rands);
}
