#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <sys/types.h>

static char *prog_name; /**> The programs name. */

typedef struct Edge {
    struct Edge *next;
    int start;
    int end;
} edge_t;

typedef struct Vertex {
    struct Vertex *next;
    int idx;
} vertex_t;

typedef struct Graph {
    struct Edge *e_head;
    struct Vertex *v_head;
} graph_t;

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

// add item if not included in linked list
void add(struct Vertex *head, int idx) {
    struct Vertex *cur = head;
    while (cur->next != NULL) {
        if (cur->idx == idx) return;
        cur = cur->next;
    }
    cur->next = malloc(sizeof(struct Vertex));
    cur->next->idx = idx;
    cur->next->next = NULL;
}


int main(int argc, char **argv) {
    prog_name = argv[0];
    if (optind >= argc) usage();
    struct Graph g = { .e_head = malloc(sizeof(edge_t*)), .v_head = malloc(sizeof(vertex_t*)) };
    for(; optind < argc; optind++){
        // TODO: format handling
        char *edge = argv[optind];
        int start, end;
        if (sscanf(edge, "%i-%i", &start, &end) < 0) usage();
        add(g.v_head, start);
        add(g.v_head, end);
        vertex_t *cur = g.v_head;
        while (cur->next != NULL) {
            printf("%i\n", cur->idx);
            cur = cur->next;
        }
    }
    return EXIT_SUCCESS;
}
