#include "shm.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <signal.h>

char *prog_name;

volatile sig_atomic_t quit = 0;

/**
 * @brief Prints the usage of the program and exists.
 * @details Usage is printed to stderr. Exits the program with EXIT_FAILURE.<br>
 * Error handling of fprintf is not covered as the program has to exit anyway.<br>
 * Used global variables: prog_name
 */
static void usage(void) {
    fprintf(stderr, "Usage: %s\n", prog_name);
    exit(EXIT_FAILURE);
}

/**
 * @brief Handles an interrupt.
 * @details Instructs the program to quit by setting the global variable quit to 1.<br>
 * Used global variables: quit
 * @param signal
 */
static void handle_interrupt(int signal) {
    if (signal == SIGINT || signal == SIGTERM) quit = 1;
}

/**
 * @brief Registers necessary sighandlers.
 */
static void register_sighandler(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_interrupt;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

/**
 * @brief Searches the smallest feedback arc set.
 * @details Reads from the circular buffer in the shared memory.
 * @param shm Pointer to the shared memory.
 * @param sem_map Pointer to the semaphore map.
 * @return 0 on success, -1 on error.
 */
static int search_smallest_fas(shm_t *shm, sem_map_t *sem_map) {
    int smallest_fac_size = FAC_MAX_LEN + 1;
    while (!quit) {
        cbi_t cbi; // malloc?
        cbi.size = FAC_MAX_LEN + 1;
        if (read_cb(shm, sem_map, &cbi) == -1) return t_err("read_cb");
        if (cbi.size == 0) {
            printf("[%s] The graph is acyclic!\n", prog_name);
            quit = 1;
        } else if (cbi.size < smallest_fac_size) {
            printf("[%s] Solution with %i edges: ", prog_name, cbi.size);
            for (int i = 0; i < cbi.size; i++) {
                printf("%i-%i ", cbi.fas[i].start, cbi.fas[i].end);
            }
            printf("\n");
            smallest_fac_size = cbi.size;
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    prog_name = argv[0];
    if (optind < argc) usage();
    int shm_fd;
    shm_t *shm;
    if (open_shm(1, &shm_fd, &shm) == -1) e_err("open_shm");
    register_sighandler();
    sem_map_t sem_map = {NULL, NULL, NULL};
    if (open_all_sem(1, &sem_map) == -1) {
        close_shm(1, shm_fd);
        e_err("open_all_sem");
    }
    if (search_smallest_fas(shm, &sem_map) == -1) {
        close_all_sem(1, &sem_map);
        close_shm(1, shm_fd);
        e_err("search_smallest_fas");
    };
    shm->active = 0;
    // free once more, in case the circular buffer has no free space and one process is waiting
    if (sem_post(sem_map.cb_free) == -1) e_err("sem_post");
    if (close_all_sem(1, &sem_map) == -1) {
        close_shm(1, shm_fd);
        e_err("close_all_sem");
    }
    if (close_shm(1, shm_fd) == -1) e_err("close_shm");
    return EXIT_SUCCESS;
}
