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
void free_rands(char *rands[R_N]) {
    for (int i = 0; i < R_N; i++) {
        if (rands[i] != NULL) free(rands[i]);
    }
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

int receive_rands(char *rands[R_N]) {
    char *line = NULL; /**< String of the current line. */
    size_t len = 0; /**< Length of the current line. */
    int line_c = 0;
    while ((getline(&line, &len, stdin)) > 0) {
        remove_newline(line);
        if (is_hex(line) == -1) {
            free(line);
            m_err("Input must be a hexadecimal number");
            return t_err("is_hex");
        }
        // TODO: find out why mem-leak if second input for example "2g"
        rands[line_c] = (char *) calloc(strlen(line) + 1, sizeof(char));
        if (rands[line_c] == NULL) {
            free(line);
            return t_err("malloc");
        }
        strcpy(rands[line_c], line);
        if (++line_c >= R_N) break;
    }
    free(line);
    if (line_c < R_N) return m_err("Less than two hexadecimal numbers provided");
    int max_length = max(strlen(rands[0]), strlen(rands[1]));
    for (int i = 0; i < R_N; i++) {
        if (fill_zeroes(&(rands[i]), max_length) < 0) t_err("fill_zeroes");
    }
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

int multiply_rands(char *rands[R_N]) {
    int status;
    int pipefd[2];
    pipe(pipefd);
    pid_t cid = fork();

    switch (cid) {
        case -1:
            return t_err("fork");
        case 0:
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);

            printf("child process\n");
            execlp("./intmul", "./intmul", "test", NULL);
            break;
        default:
            close(pipefd[0]);
            //dup2(pipefd[1], STDOUT_FILENO);
            //close(pipefd[1]);

            FILE *f = fdopen(pipefd[1], "w");
            if (f == NULL) t_err("fdopen");
            if (fputs("3\n", f) == -1) return t_err("fputs");
            if (fputs("2\n", f) == -1) return t_err("fputs");
            fflush(f);
            fclose(f);

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
    char *rands[R_N] = {NULL, NULL}; /**< Operands to be multiplied. */
    if (receive_rands(rands) < 0) {
        free_rands(rands);
        e_err("receive_rands");
    }
    int rand_length = strlen(rands[0]);
    if (rand_length == 1) {
        printf("%lX\n", strtol(rands[0], NULL, 16) * strtol(rands[1], NULL, 16));
        printf("success\n");
    } else {
        if (multiply_rands(rands) < 0) {
            free_rands(rands);
            e_err("multiply_rands");
        };
    }
    free_rands(rands);
    return EXIT_SUCCESS;
}
