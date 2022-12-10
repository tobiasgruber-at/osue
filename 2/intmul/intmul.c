/**
 * Intmul module.
 * @brief Main entry point for the intmul program.
 * @details Efficiently performs a multiplication of two hexadecimal numbers of any length.<br>
 * To split up and accelerate the computation, it recursively creates child processes that calculate parts of the
 * multiplication.<br>
 * Numbers are read from <strong>stdin</strong> and outputted to <strong>stdout</strong>.
 * @file intmul.c
 * @author Tobias Gruber, 11912367
 * @date 18.11.2022
 **/

#include "hex.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#define R_N 2 /**< Number of operands for the multiplication. */
#define F_N 4 /**< Number of forked child processes. */
#define P_N 2 /**< Number of pipe ends. */

char *prog_name;

/**
 * @brief Prints the usage of the program and exits.
 * @details Prints to <strong>stderr</strong> and exits with <strong>EXIT_FAILURE</strong>.<br>
 * Used global variables: prog_name
 */
static void usage(void) {
    fprintf(stderr, "Usage: %s\n", prog_name);
    exit(EXIT_FAILURE);
}

/**
 * @brief Waits for all given processes to terminate.
 * @param pid Array of all process ids. -1 as id indicates, that is was not set yet.
 * @return 0 on success, -1 on error.
 */
static int wait_all(pid_t pid[F_N])
{
    int err = 0;
    for (size_t i = 0; i < F_N; i++)
    {
        if (pid[i] == -1) continue;
        int status;
        if (waitpid(pid[i], &status, 0) < 0 || WEXITSTATUS(status) != EXIT_SUCCESS) err = 1;
    }
    if (err == 1) return m_err("waitpid");
    return 0;
}

/**
 * @brief Frees allocated memory of the operands.
 * @param a First operand.
 * @param b Second operand.
 */
static void free_rands(char *a, char *b) {
    if (a != NULL) free(a);
    if (b != NULL) free(b);
}

/**
 * @brief Reads the operands from <strong>stdin</strong>.
 * @details Validates the operands as hexadecimal numbers and transforms them
 * so that their lengths are equal and a power of two.<br>
 * Allocates the necessary memory for <strong>a</strong> and <strong>b</strong>.
 * @param a Pointer to be updated with the first operand string.
 * @param b Pointer to be updated with the second operand string.
 * @return 0 on success, -1 on error.
 */
static int receive_rands(char **a, char **b) {
    char *line = NULL; /**< String of the current line. */
    size_t len = 0; /**< Line length. */
    size_t line_c = 0; /**< Line counter. */
    while ((getline(&line, &len, stdin)) > 0) {
        char **x = (line_c == 0) ? a : b; /** Current operand. */
        remove_newline(line);
        if (is_hex(line) == -1) {
            free(line);
            m_err("Input must be a hexadecimal number");
            errno = EINVAL;
            return t_err("is_hex");
        }
        *x = (char *) malloc(sizeof(char) * (strlen(line) + 1));
        if (*x == NULL) {
            free(line);
            return t_err("malloc");
        }
        strcpy(*x, line);
        if (++line_c >= R_N) break;
    }
    free(line);
    if (line_c < R_N) {
        errno = EINVAL;
        return m_err("Less than two hexadecimal numbers provided");
    }
    int max_len = max(strlen(*a), strlen(*b)); /**< Max length of both operands. */
    if (
        fill_zeroes(a, max_len, true) < 0 ||
        fill_zeroes(b, max_len, true) < 0
    ) t_err("fill_zeroes");
    return 0;
}

/**
 * @brief Delegates a hex multiplication to a forked child.
 * @details Forks the process and communicates the input and output of the child with pipes.<br>
 * Allocates necessary memory for <strong>res</strong>.<br>
 * Used global variables: prog_name
 * @param res Pointer to be updated with the result string.
 * @param x String of the first operand (in hex).
 * @param y String of the second operand (in hex).
 * @param cid Pointer to be updated with the child's process id.
 * @return 0 on success, -1 on error.
 */
static int fork_child(char **res, pid_t *cid, char *x, char *y) {
    /**
     * Pipe to pass input to child as an array.
     * @details First element is read end, second one the write end.
     */
    int pin_fd[P_N];
    /**
     * Pipe to pass output to parent as an array.
     * @details First element is read end, second one the write end.
     */
    int pout_fd[P_N];
    if (pipe(pin_fd) == -1) return t_err("pipe");
    if (pipe(pout_fd) == -1) return t_err("pipe");
    *cid = fork();
    switch (*cid) {
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
            execlp(prog_name, prog_name, NULL);
            return t_err("execlp");
        default:
            close(pin_fd[0]);
            close(pout_fd[1]);

            FILE *pin_p = fdopen(pin_fd[1], "w"); /**< Pointer to the input pipe. */
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

            FILE *pout_p = fdopen(pout_fd[0], "r"); /**< Pointer to the output pipe. */
            if (pout_p == NULL) t_err("fdopen");
            char *line = NULL;
            size_t len = 0;
            if ((getline(&line, &len, pout_p)) == -1) {
                free(line);
                fclose(pout_p);
                close(pout_fd[0]);
                return t_err("getline");
            }
            fflush(pout_p);
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
    }
    return 0;
}

/**
 * @brief Multiplies two hex numbers of any length recursively.
 * @details Output is printed to <strong>stdout</strong>.<br>
 * Delegates parts of the calculation to four child processes and brings them together to receive the product.<br>
 * The parent process is waiting until all children are done.<br>
 * The operands are not limited in length.
 * @param a String of the first operand (in hex).
 * @param b String of the second operand (in hex).
 * @return 0 on success, -1 on error.
 */
static int multiply_recursively(char *a, char *b) {
    int len = strlen(a); /**< Length of the operands. */
    int half_len = len / 2; /**< Half length of the operands. */
    char a_h[half_len + 1], a_l[half_len + 1], b_h[half_len + 1], b_l[half_len + 1];
    half_str(a_h, a, 0, half_len);
    half_str(a_l, a, 1, half_len);
    half_str(b_h, b, 0, half_len);
    half_str(b_l, b, 1, half_len);
    char *res[F_N] = { NULL, NULL, NULL, NULL }; /** Responses of the children. */
    pid_t pid[F_N] = { -1, -1, -1, -1 }; /** Process ids of the children. */
    if (
        fork_child(&(res[0]), &(pid[0]), a_h, b_h) == -1 ||
        fork_child(&(res[1]), &(pid[1]), a_h, b_l) == -1 ||
        fork_child(&(res[2]), &(pid[2]), a_l, b_h) == -1 ||
        fork_child(&(res[3]), &(pid[3]), a_l, b_l) == -1
    ) {
        wait_all(pid);
        free_arr(res, F_N);
        return t_err("fork_child");
    }
    if (wait_all(pid) == -1) {
        free_arr(res, F_N);
        return t_err("wait_all");
    }
    if (
        shift_left(&(res[0]), len) == -1 ||
        shift_left(&(res[1]), half_len) == -1 ||
        shift_left(&(res[2]), half_len) == -1
    ) {
        free_arr(res, F_N);
        return t_err("shift_left");
    }
    char *prod = NULL; /**< Product of the multiplication. */
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

/**
 * @brief Performs a multiplication of two hexadecimal numbers.
 * @details Reads two numbers of any length as operands from <strong>stdin</strong> and outputs the product to
 * <strong>stdout</strong>.<br>
 * Generates child processes that recursively call this program to split up the calculation work.<br>
 * Child processes communicate with their parents by pipes.<br>
 * Takes no arguments.<br>
 * If an error occurs it exits with <strong>EXIT_FAILURE</strong>.
 * @param argc Argument counter.
 * @param argv Argument vector.
 * @return <strong>EXIT_SUCCESS</strong> on successful termination.
 */
int main(int argc, char **argv) {
    prog_name = argv[0];
    if (argc > 1) usage();
    char *a = NULL, *b = NULL; /**< Operands to be multiplied. */
    if (receive_rands(&a, &b) < 0) {
        free_rands(a, b);
        e_err("receive_rands");
    }
    if (strlen(a) == 1) {
        char *prod_hex;
        if (multiply(&prod_hex, a, b) == -1) {
            free(prod_hex);
            free_rands(a, b);
            e_err("multiply");
        } else {
            printf("%s\n", prod_hex);
            free(prod_hex);
            fflush(stdout);
        };
    } else if (multiply_recursively(a, b) == -1) {
        free_rands(a, b);
        e_err("multiply_recursively");
    }
    free_rands(a, b);
    return EXIT_SUCCESS;
}
