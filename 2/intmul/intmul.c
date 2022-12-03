#include "misc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

// count of operands
#define R_N 2 /**< Number of operands (for the multiplication) */

char *prog_name;

void usage() {
    fprintf(stderr, "Usage: %s\n", prog_name);
    exit(EXIT_FAILURE);
}

/** Frees allocated memory for the operands. */
void free_rands(char *a, char *b) {
    if (a != NULL) free(a);
    if (b != NULL) free(b);
}

/**
 * Fills up a hexadecimal number with leading zeroes, until it's length is a power of two and at least a given
 * min-length long.
 */
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

/** Gets operands and mallocs memory.. */
int receive_rands(char **a, char **b) {
    char *line = NULL; /**< String of the current line. */
    size_t len = 0, line_c = 0;
    while ((getline(&line, &len, stdin)) > 0) {
        char **X = (line_c == 0) ? a : b;
        remove_newline(line);
        if (is_hex(line) == -1) {
            free(line);
            m_err("Input must be a hexadecimal number");
            return t_err("is_hex");
        }
        // TODO: find out why mem-leak if second input for example "2g"
        *X = (char *) malloc(sizeof(char) * (strlen(line) + 1));
        if (*X == NULL) {
            free(line);
            return t_err("malloc");
        }
        strcpy(*X, line);
        if (++line_c >= R_N) break;
    }
    free(line);
    if (line_c < R_N) return m_err("Less than two hexadecimal numbers provided");
    int max_length = max(strlen(*a), strlen(*b));
    if (fill_zeroes(a, max_length) < 0) t_err("fill_zeroes");
    if (fill_zeroes(b, max_length) < 0) t_err("fill_zeroes");
    return 0;
}

int multiply(char *rands[R_N], int *res) {
    if (strlen(rands[0]) == 1) {
        //*res = rands[0] * rands[1];
    }
    return 0;
}

int dec_to_hex(int decVal, char **res) {
    int base = 1;
    int len = 0;
    while (decVal / base > 0) {
        base *= 16;
        len++;
    }
    *res = (char *) malloc(sizeof(char) * (len + 1));
    base /= 16;
    for (int i = 0; i < len; i++) {
        int offset;
        int digit = decVal / base;
        if (digit >= 0 && digit <= 9) offset = 48;
        else if (digit >= 10 && digit <= 15) offset = 55;
        else {
            free(res);
            return m_err("Invalid decimal input");
        }
        (*res)[i] = digit + offset;
        decVal %= base;
        base /= 16;
    }
    (*res)[len] = '\0';
    return 0;
}

/** 
 * Copies half of a string to another string.
 * @param dst Destination string.
 * @param src Source string.
 * @param half 0 if the first half should be copied, 1 for the second half.
 * @param half_length Half length of the source string.
 */
void half_str(char *dst, char *src, int half, int half_length) {
    char *start = src;
    if (half == 1) start += half_length;
    strcpy(dst, start);
    if (half == 0) dst[half_length] = '\0';
}

int multiply_rands(char *a, char *b) {
    int half_length = strlen(a) / 2;
    char a_h[half_length + 1], a_l[half_length + 1], b_h[half_length + 1], b_l[half_length + 1];
    half_str(a_h, a, 0, half_length);
    half_str(a_l, a, 1, half_length);
    half_str(b_h, b, 0, half_length);
    half_str(b_l, b, 1, half_length);
    int status;
    int pin_fd[2];
    int pout_fd[2];
    if (pipe(pin_fd) == -1) return t_err("pipe");
    if (pipe(pout_fd) == -1) return t_err("pipe");
    pid_t cid = fork();
    switch (cid) {
        case -1:
            return t_err("fork");
        case 0:
            close(pin_fd[1]);
            close(pout_fd[0]);
            if (dup2(pin_fd[0], STDIN_FILENO) == -1) return t_err("dup2");
            if (dup2(pout_fd[1], STDOUT_FILENO) == -1) return t_err("dup2");
            close(pin_fd[0]);
            close(pout_fd[1]);
            execlp("./intmul", "./intmul", NULL);
            return t_err("execlp");
        default:
            close(pin_fd[0]);
            close(pout_fd[1]);

            FILE *pin_p = fdopen(pin_fd[1], "w");
            if (pin_p == NULL) t_err("fdopen");
            if (fprintf(pin_p, "%s\n", a_l) == -1) return t_err("fputs");
            if (fprintf(pin_p, "%s\n", b_l) == -1) return t_err("fputs");
            fflush(pin_p);

            FILE *pout_p = fdopen(pout_fd[0], "r");
            if (pout_p == NULL) t_err("fdopen");
            char *result = NULL;
            size_t len = 0;
            if ((getline(&result, &len, pout_p)) == -1) return t_err("getline");
            printf("%s\n", result);

            free(result);
            fclose(pin_p);
            fclose(pout_p);
            close(pin_fd[1]);
            close(pout_fd[0]);

            waitpid(cid, &status, 0);
            if (WEXITSTATUS(status) == EXIT_SUCCESS) {

            } else {
                return t_err("waitpid");
            }
            break;
    }
    return 0;
}

int main(int argc, char **argv) {
    prog_name = argv[0];
    if (argc > 1) usage();
    char *a = NULL, *b = NULL; /**< Operands to be multiplied. */
    if (receive_rands(&a, &b) < 0) {
        free_rands(a, b);
        e_err("receive_rands");
    }
    if (strlen(a) == 1) {
        printf("%lX\n", strtol(a, NULL, 16) * strtol(b, NULL, 16));
        fflush(stdout);
    } else {
        if (multiply_rands(a, b) < 0) {
            free_rands(a, b);
            e_err("multiply_rands");
        };
    }
    free_rands(a, b);
    return EXIT_SUCCESS;
}
