#include "graph.h"
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <sys/types.h>

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

int main(int argc, char **argv) {
    prog_name = argv[0];
    if (optind >= argc) usage();
    int edges_upper = argc - optind; /**< Upper bound for edges count. */
    int vertices_upper = edges_upper * 2; /**< Upper bound for vertices count. */
    struct Graph g = {
        malloc(sizeof(edge_t*) * edges_upper),
        malloc(sizeof(vertex_t*) * vertices_upper)
    };
    for(; optind < argc; optind++){
        // TODO: format handling
        char *edge = argv[optind];
        int start, end;
        if (sscanf(edge, "%i-%i", &start, &end) < 0) usage();
        add_edge(&g, start, end);
    }
    if (g.edges_count < edges_upper) {
        g.edges = realloc(g.edges, sizeof(edge_t*) * g.edges_count);
    }
    if (g.vertices_count < vertices_upper) {
        g.vertices = realloc(g.vertices, sizeof(vertex_t*) * g.vertices_count);
    }
    print_graph(&g);
    free_graph(&g);
    return EXIT_SUCCESS;
}
