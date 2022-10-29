#include "shm.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>

static char *prog_name; /**> The programs name. */

volatile sig_atomic_t quit = 0;

void handle_interrupt(int signal) {
    quit = 1;
}

void register_sighandler(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_interrupt;
    sigaction(SIGINT, &sa, NULL);
}

/**
 * @brief Prints the usage of the program to stderr and exists with an error.
 * @details Exits the program with EXIT_FAILURE.<br>
 * Error handling of fprintf is not covered as the program has to exit anyway.<br>
 * Used global variables: prog_name
 */
static void usage(void) {
    fprintf(stderr, "Usage: %s\n", prog_name);
    exit(EXIT_FAILURE);
}

static void exit_err(char *fun_name) {
    fprintf(stderr, "[%s] %s failed: %s\n", prog_name, fun_name, strerror(errno));
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv) {
    register_sighandler();
    prog_name = argv[0];
    if (optind < argc) usage();
    int shm_fd = 0;
    shm_t *shm;
    if (open_shm(1, &shm_fd, &shm) == -1) exit_err("init_shm");
    sem_t *sem;
    if (open_sem(1, &sem) == -1) {
        close_shm(1, shm_fd, sizeof(*shm));
        exit_err("init_sem");
    }
    int i = 0;
    while (!quit) {
        sleep(1);
        shm->data[0] = i++;
        printf("%i\n", shm->data[0]);
    }
    if (close_sem(1, sem) == -1) {
        close_shm(1, shm_fd, sizeof(*shm));
        exit_err("destroy_sem");
    }
    if (close_shm(1, shm_fd, sizeof(*shm)) == -1) exit_err("destroy_shm");
    return EXIT_SUCCESS;
}
