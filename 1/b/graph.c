#include "graph.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Adds a vertex to a graph.
 * @details Only adds if it does not already exist.
 */
static void add_vertex(graph_t *g, int idx) {
    for (int i = 0; i < g->vertices_count; i++) {
        if (g->vertices[i]->idx == idx) return;
    }
    vertex_t **new = &g->vertices[g->vertices_count++];
    *new = (vertex_t*) malloc(sizeof(vertex_t));
    (*new)->idx = idx;
}

void add_edge(graph_t *g, int start, int end) {
    for (int i = 0; i < g->edges_count; i++) {
        edge_t *cur = g->edges[i];
        if (cur->start == start && cur->end == end) return;
    }
    edge_t **new = &g->edges[g->edges_count++];
    *new = (edge_t*) malloc(sizeof(edge_t));
    (*new)->start = start;
    (*new)->end = end;
    add_vertex(g, start);
    add_vertex(g, end);
}

void print_graph(graph_t *g) {
    printf("Vertices: ");
    for (int i = 0; i < g->vertices_count; i++) {
        printf("%i, ", g->vertices[i]->idx);
    }
    printf("\nEdges: ");
    for (int i = 0; i < g->edges_count; i++) {
        edge_t *cur = g->edges[i];
        printf("(%i-%i), ", cur->start, cur->end);
    }
    printf("\n");
}

void free_graph(graph_t *g) {
    for (int i = 0; i < g->vertices_count; i++) {
        free(g->vertices[i]);
    }
    free(g->vertices);
    for (int i = 0; i < g->edges_count; i++) {
        free(g->edges[i]);
    }
    free(g->edges);
}
