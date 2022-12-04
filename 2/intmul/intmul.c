#include "hex.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <math.h>

// count of operands
#define R_N 2 /**< Number of operands (for the multiplication) */
#define F_N 4 /**< Number of forked processes (for the multiplication) */

char *prog_name;

static void usage() {
    fprintf(stderr, "Usage: %s\n", prog_name);
    exit(EXIT_FAILURE);
}

/** Frees allocated memory for the operands. */
static void free_rands(char *a, char *b) {
    if (a != NULL) free(a);
    if (b != NULL) free(b);
}

/** Gets operands and mallocs memory.. */
static int receive_rands(char **a, char **b) {
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
    if (fill_zeroes(a, max_length, true) < 0) t_err("fill_zeroes");
    if (fill_zeroes(b, max_length, true) < 0) t_err("fill_zeroes");
    return 0;
}

/**
 * Delegates a part of the computation to a forked child.
 * @param res
 * @param x
 * @param y
 */
static int fork_child(char **res, char *x, char *y) {
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
            if (
                dup2(pin_fd[0], STDIN_FILENO) == -1 ||
                dup2(pout_fd[1], STDOUT_FILENO) == -1
            ) return t_err("dup2");
            close(pin_fd[0]);
            close(pout_fd[1]);
            execlp("./intmul", "./intmul", NULL);
            return t_err("execlp");
        default:
            close(pin_fd[0]);
            close(pout_fd[1]);

            FILE *pin_p = fdopen(pin_fd[1], "w");
            if (pin_p == NULL) t_err("fdopen");
            if (
                fprintf(pin_p, "%s\n", x) == -1 ||
                fprintf(pin_p, "%s\n", y) == -1
            ) {
                fclose(pin_p);
                close(pin_fd[1]);
                close(pout_fd[0]);
                return t_err("fprintf");
            }
            fflush(pin_p);
            fclose(pin_p);
            close(pin_fd[1]);

            FILE *pout_p = fdopen(pout_fd[0], "r");
            if (pout_p == NULL) t_err("fdopen");
            char *line = NULL;
            size_t len = 0;
            if ((getline(&line, &len, pout_p)) == -1) {
                free(line);
                fclose(pout_p);
                close(pout_fd[0]);
                return t_err("getline");
            }
            fclose(pout_p);
            close(pout_fd[0]);

            *res = (char *) malloc(sizeof(char) * (strlen(line) + 1));
            if (*res == NULL) {
                free(line);
                return t_err("malloc");
            }
            strcpy(*res, line);
            remove_newline(*res);
            free(line);

            int status;
            waitpid(cid, &status, 0);
            // TODO: handle wait for all processes at once
            if (WEXITSTATUS(status) != EXIT_SUCCESS) return t_err("waitpid");
            break;
    }
    return 0;
}

/**
 * Evaluates the hex product.
 * @param prod_dec Product as a decimal number.
 * @param length Length of the hex product.
 */
static int evaluate_prod(int prod_dec, int length) {
    char *prod_hex = (char *) malloc(sizeof(char) * (length + 1));
    if (prod_hex == NULL) return t_err("malloc");
    sprintf(prod_hex, "%x", prod_dec);
    fill_zeroes(&prod_hex, length, false);
    printf("%s\n", prod_hex);
    free(prod_hex);
    return 0;
}

/** Multiplies two hex numbers. */
static int fork_multiply(char *a, char *b) {
    int len = strlen(a), half_len = len / 2;
    char a_h[half_len + 1], a_l[half_len + 1], b_h[half_len + 1], b_l[half_len + 1];
    half_str(a_h, a, 0, half_len);
    half_str(a_l, a, 1, half_len);
    half_str(b_h, b, 0, half_len);
    half_str(b_l, b, 1, half_len);
    char *res[F_N] = { NULL, NULL, NULL, NULL };
    if (
        fork_child(&(res[0]), a_h, b_h) == -1 ||
        fork_child(&(res[1]), a_h, b_l) == -1 ||
        fork_child(&(res[2]), a_l, b_h) == -1 ||
        fork_child(&(res[3]), a_l, b_l) == -1
    ) {
        free_arr(res, F_N);
        return t_err("fork_child");
    }
    if (
        shift_left(&(res[0]), len) == -1 ||
        shift_left(&(res[1]), half_len) == -1 ||
        shift_left(&(res[2]), half_len) == -1
    ) {
        free_arr(res, F_N);
        return t_err("shift_left");
    }
    char *prod = NULL;
    if (
        add(&prod, &(res[0]), &(res[1])) == -1 ||
        add(&prod, &prod, &(res[2])) == -1 ||
        add(&prod, &prod, &(res[3])) == -1
    ) {
        free_arr(res, F_N);
        return t_err("add");
    }
    printf("%s\n", prod);
    free_arr(res, F_N);
    free(prod);
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
        long prod_dec; /**< Decimal line of a hex multiplication. */
        multiply(&prod_dec, a, b);
        evaluate_prod(prod_dec, strlen(a) * 2);
        fflush(stdout);
    } else if (fork_multiply(a, b) < 0) {
        free_rands(a, b);
        e_err("fork_multiply");
    }
    free_rands(a, b);
    return EXIT_SUCCESS;
}
