/**
 * Generator module.
 * @brief Main entry point for generators.
 * @details Takes a graph as an input and continuously generates feedback arc sets for this graph that are
 * communicated to the supervisor.<br>
 * Must only be started while the supervisor is running.<br>
 * Terminates when the supervisor notifies to stop.
 * @file generator.c
 * @author Tobias Gruber, 11912367
 * @date 23.10.2022
 **/

#include "shm.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <string.h>

char *prog_name;

/**
 * @brief Prints the usage of the program and exits.
 * @details Prints to stderr and exits with EXIT_FAILURE.<br>
 * Used global variables: prog_name
 */
static void usage(void) {
    fprintf(stderr, "Usage: %s edge...\nEXAMPLE: %s 0-1 1-2 1-3 1-4 2-4 3-6 4-3 4-5 6-0\n", prog_name, prog_name);
    exit(EXIT_FAILURE);
}

/**
 * @brief Initialises the graph.
 * @details Constructs the graph by parsing the program's arguments.
 * @param g Pointer to the graph that will be created.
 * @param argc Program's argument counter.
 * @param argv Program's argument vector.
 * @return 0 on success, -1 on error.
 */
static int init_graph(graph_t *g, int argc, char **argv) {
    int edges_upper = argc - optind; /**< Upper bound for edges count. */
    int vertices_upper = edges_upper * 2; /**< Upper bound for vertices count. */
    g->edges = (edge_t**) malloc(sizeof(edge_t*) * edges_upper);
    if (g->edges == NULL) return t_err("malloc");
    g->vertices = (int*) malloc(sizeof(int) * vertices_upper);
    if (g->vertices == NULL) {
        free_graph(g);
        return t_err("malloc");
    }
    for(; optind < argc; optind++){
        char *input = argv[optind];
        edge_t e = {0, 0};
        if (parse_int(&e.start, strtok(input, "-")) == -1) {
            free_graph(g);
            t_err("parse_int");
            usage();
        }
        if (parse_int(&e.end, strtok(NULL, "")) == -1) {
            free_graph(g);
            t_err("parse_int");
            usage();
        }
        if (add_edge(g, &e) == -1) {
            free_graph(g);
            e_err("add_edge");
        }
    }
    if (g->edges_count < edges_upper) {
        g->edges = (edge_t**) realloc(g->edges, sizeof(edge_t*) * g->edges_count);
    }
    if (g->vertices_count < vertices_upper) {
        g->vertices = (int*) realloc(g->vertices, sizeof(int) * g->vertices_count);
    }
    return 0;
}

/**
 * @brief Generates a feedback arc set of a graph.
 * @details The randomized algorithm always finds a feedback arc set by:<ol>
 * <li>Receiving a random permutation of the graph's edges by shuffling them.</li>
 * <li>Adding all edges (u, v) for which u > v to the feedback arc set.</li></ol>
 * @param g Pointer to source graph.
 * @param fas Pointer to pointer to the generated feedback arc set. Pointed pointer must reserve enough memory.
 * @param size Pointer to the size of the generated feedback arc set.
 */
static void generate_fas(graph_t *g, edge_t **fas, int *size) {
    shuffle(g->vertices, g->vertices_count);
    *size = 0;
    for (int i = 0; i < g->edges_count; i++) {
        edge_t *e = g->edges[i];
        if (is_after(g->vertices, g->edges_count, e) > 0) {
            fas[(*size)++] = e;
        }
    }
}

/**
 * @brief Creates feedback arc sets and writes them to the shared memory.
 * @details Continuously creates feedback arc sets of a graph and pushes them to the circular buffer in the
 * shared memory.<br>
 * Runs until the supervisor notifies to stop.
 * @param g Pointer to the graph.
 * @param shm Pointer to the shared memory.
 * @param sem_map Pointer to the semaphore map.
 * @return 0 on success, -1 on error.
 */
static int generate_smallest_fas(graph_t *g, shm_t *shm, sem_map_t *sem_map) {
    srand(getpid());
    edge_t **fas = (edge_t**) malloc(sizeof(edge_t*) * g->edges_count);
    if (fas == NULL) return t_err("malloc");
    while (shm->active == 1) {
        int fas_size = 0;
        generate_fas(g, fas, &fas_size);
        if (fas_size <= FAC_MAX_LEN) {
            cbi_t cbi;
            cbi.size = fas_size;
            for (int i = 0; i < fas_size; i++) {
                cbi.fas[i] = *fas[i];
            }
            if (push_cb(cbi, shm, sem_map) == -1) {
                free(fas);
                return t_err("push_cb");
            }
        }
    }
    free(fas);
    return 0;
}

/**
 * @brief Main function for the generator program.
 * @details Parses the given graph from the arguments and continuously generates feedback arc sets that are
 * stored in the circular buffer of the shared memory.<br>
 * Necessary shared memory and semaphores are opened and closed afterwards to accomplish this communication.<br>
 * If an error occurs it exits with EXIT_FAILURE.
 * @param argc Argument counter.
 * @param argv Argument vector.
 * @return EXIT_SUCCESS on successful termination.
 */
int main(int argc, char **argv) {
    prog_name = argv[0];
    if (optind >= argc) usage();
    int shm_fd;
    shm_t *shm;
    if (open_shm(0, &shm_fd, &shm) == -1) e_err("open_shm");
    sem_map_t sem_map = {NULL, NULL, NULL};
    if (open_all_sem(0, &sem_map) == -1) {
        close_shm(0, shm_fd);
        e_err("open_all_sem");
    }
    struct Graph g = {NULL, NULL, 0, 0};
    if (init_graph(&g, argc, argv) == -1) {
        close_all_sem(0, &sem_map);
        close_shm(0, shm_fd);
        e_err("init_graph");
    }
    if (generate_smallest_fas(&g, shm, &sem_map) == -1) {
        free_graph(&g);
        close_all_sem(0, &sem_map);
        close_shm(0, shm_fd);
        e_err("generate_smallest_fas");
    }
    free_graph(&g);
    if (close_all_sem(0, &sem_map) < 0) {
        close_shm(0, shm_fd);
        e_err("close_all_sem");
    }
    if (close_shm(0, shm_fd) == -1) e_err("close_shm");
    return EXIT_SUCCESS;
}
