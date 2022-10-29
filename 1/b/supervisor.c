#include "shm.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <signal.h>

static char *prog_name; /**> The programs name. */

volatile sig_atomic_t quit = 0;

void handle_interrupt(int signal) {
    quit = 1;
}

void register_sighandler() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_interrupt;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
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
    prog_name = argv[0];
    if (optind < argc) usage();
    int shm_fd;
    shm_t *shm;
    if (open_shm(1, &shm_fd, &shm) == -1) exit_err("open_shm");
    register_sighandler();
    sem_map_t sem_map = {NULL, NULL, NULL};
    if (open_all_sem(1, &sem_map) == -1) {
        close_shm(1, shm_fd);
        exit_err("open_all_sem");
    }
    while (!quit) {
        read_cb(shm, &sem_map, NULL);
    }
    shm->active = 0;
    if (close_all_sem(1, &sem_map) == -1) {
        close_shm(1, shm_fd);
        exit_err("close_all_sem");
    }
    if (close_shm(1, shm_fd) == -1) exit_err("close_shm");
    return EXIT_SUCCESS;
}
