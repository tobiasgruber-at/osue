#include "shm.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>

static char *prog_name; /**> The programs name. */

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

static void exit_err(char *fun_name) {
    fprintf(stderr, "[%s] %s failed: %s\n", prog_name, fun_name, strerror(errno));
    exit(EXIT_FAILURE);
}

static int init_graph(graph_t *g, int argc, char **argv) {
    int edges_upper = argc - optind; /**< Upper bound for edges count. */
    int vertices_upper = edges_upper * 2; /**< Upper bound for vertices count. */
    g->edges = (edge_t**) malloc(sizeof(edge_t*) * edges_upper);
    if (g->edges == NULL) return -1;
    g->vertices = (int*) malloc(sizeof(int) * vertices_upper);
    if (g->edges == NULL) return -1;
    for(; optind < argc; optind++){
        // TODO: format handling
        char *edge = argv[optind];
        int start, end;
        if (sscanf(edge, "%i-%i", &start, &end) < 0) usage();
        add_edge(g, start, end);
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
 * Gets the feedback arc set (fas) of a graph.
 * @details fas must reserve as much memory as the graphs' edges.<br>
 * TODO: describe algorithm
 * @param g Pointer to source graph
 * @param fas Pointer to fas
 * @param size Pointer to size of fas
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

static int search_smallest_fas(graph_t *g, shm_t *shm, sem_map_t *sem_map) {
    edge_t **fas = (edge_t**) malloc(sizeof(edge_t*) * g->edges_count);
    if (fas == NULL) return -1;
    while (shm->active == 1) {
        shuffle(g->vertices, g->vertices_count);
        int fas_size = 0;
        generate_fas(g, fas, &fas_size);
        if (fas_size <= FAC_MAX_LEN) {
            if (push_cb(NULL, shm, sem_map) == -1) exit_err("push_cb");
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
    if (open_shm(0, &shm_fd, &shm) == -1) exit_err("open_shm");
    sem_map_t sem_map = {NULL, NULL, NULL};
    if (open_all_sem(0, &sem_map) == -1) {
        close_shm(0, shm_fd);
        exit_err("open_all_sem");
    }
    struct Graph g = {NULL, NULL};
    if (init_graph(&g, argc, argv) == -1) {
        close_all_sem(0, &sem_map);
        exit_err("init_graph");
    }
    srand(getpid());
    if (search_smallest_fas(&g, shm, &sem_map) == -1) {
        close_all_sem(0, &sem_map);
        free_graph(&g);
        exit_err("search_smallest_fas");
    }
    free_graph(&g);
    if (close_all_sem(0, &sem_map) < 0) {
        close_shm(0, shm_fd);
        exit_err("close_all_sem");
    }
    if (close_shm(0, shm_fd) == -1) exit_err("close_shm");
    return EXIT_SUCCESS;
}
