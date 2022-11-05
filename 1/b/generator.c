#include "shm.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>

char *prog_name;

/**
 * @brief Prints the usage of the program to stderr and exists with an error.
 * @details Exits the program with EXIT_FAILURE.<br>
 * Error handling of fprintf is not covered as the program has to exit anyway.<br>
 * Used global variables: prog_name
 */
static void usage(void) {
    fprintf(stderr, "Usage: %s edge...\nEXAMPLE: %s 0-1 1-2 1-3 1-4 2-4 3-6 4-3 4-5 6-0\n", prog_name, prog_name);
    exit(EXIT_FAILURE);
}

/**
 * @brief Initialised the graph.
 * @details Constructs the graph by interpreting the programs' arguments.
 * @param g Pointer to the graph that will be updated.
 * @param argc Programs' argument counter.
 * @param argv Programs' argument vector.
 * @return 0 on success, -1 on error.
 */
static int init_graph(graph_t *g, int argc, char **argv) {
    int edges_upper = argc - optind; /**< Upper bound for edges count. */
    int vertices_upper = edges_upper * 2; /**< Upper bound for vertices count. */
    g->edges = (edge_t**) malloc(sizeof(edge_t*) * edges_upper);
    if (g->edges == NULL) return t_err("malloc");
    g->vertices = (int*) malloc(sizeof(int) * vertices_upper);
    if (g->edges == NULL) return t_err("malloc");
    for(; optind < argc; optind++){
        char *edge = argv[optind];
        edge_t e = {0, 0};
        // TODO: format handling - use strtol
        if (sscanf(edge, "%i-%i", &e.start, &e.end) < 0) usage();
        add_edge(g, &e);
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
 * @brief Gets a feedback arc set of a graph.
 * @details The feedback arc set must reserve as much memory as the graphs' edges.<br>
 * TODO: describe algorithm
 * @param g Pointer to source graph
 * @param fas Pointer to the feedback arc set.
 * @param size Pointer that will be updated with the size of the feedback arc set.
 */
static void generate_fas(graph_t *g, edge_t **fas, int *size) {
    *size = 0;
    for (int i = 0; i < g->edges_count; i++) {
        edge_t *e = g->edges[i];
        if (is_after(g->vertices, g->edges_count, e) > 0) {
            fas[(*size)++] = e;
        }
    }
}

/**
 * @brief Searches for the smallest feedback arc set.
 * @details Continuously looks for feedback arc sets and pushes them to the circular buffer in the shared memory.
 * @param g Pointer to the graph.
 * @param shm Pointer to the shared memory.
 * @param sem_map Pointer to the semaphore map.
 * @return 0 on success, -1 on error.
 */
static int search_smallest_fas(graph_t *g, shm_t *shm, sem_map_t *sem_map) {
    edge_t **fas = (edge_t**) malloc(sizeof(edge_t*) * g->edges_count);
    if (fas == NULL) return t_err("malloc");
    while (shm->active == 1) {
        shuffle(g->vertices, g->vertices_count);
        int fas_size = 0;
        generate_fas(g, fas, &fas_size);
        if (fas_size <= FAC_MAX_LEN) {
            cbi_t cbi;
            cbi.size = fas_size;
            for (int i = 0; i < fas_size; i++) {
                cbi.fas[i] = *fas[i];
            }
            // TODO: check why sometimes not stopping: e.g../generator 1-2 2-3 3-4 4-5 5-6 6-7 7-8 8-9 9-10
            if (push_cb(cbi, shm, sem_map) == -1) {
                free(fas);
                return t_err("push_cb");
            }
        }
    }
    free(fas);
    return 0;
}

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
    struct Graph g = {NULL, NULL};
    if (init_graph(&g, argc, argv) == -1) {
        close_all_sem(0, &sem_map);
        e_err("init_graph");
    }
    srand(getpid());
    if (search_smallest_fas(&g, shm, &sem_map) == -1) {
        close_all_sem(0, &sem_map);
        free_graph(&g);
        e_err("search_smallest_fas");
    }
    free_graph(&g);
    if (close_all_sem(0, &sem_map) < 0) {
        close_shm(0, shm_fd);
        e_err("close_all_sem");
    }
    if (close_shm(0, shm_fd) == -1) e_err("close_shm");
    return EXIT_SUCCESS;
}
