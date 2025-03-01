/**
 * Graph module.
 * @brief Implementation of the graph module definitions.
 * @file graph.c
 * @author Tobias Gruber, 11912367
 * @date 23.10.2022
 **/

#include "graph.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Adds a vertex to a graph.
 * @details Only adds the vertex if it wasn't already added.
 * @param g Pointer to the graph.
 * @param idx Vertex to be added.
 */
static void add_vertex(graph_t *g, int idx) {
    for (int i = 0; i < g->vertices_count; i++) {
        if (g->vertices[i] == idx) return;
    }
    g->vertices[g->vertices_count++] = idx;
}

int add_edge(graph_t *g, edge_t *e) {
    for (int i = 0; i < g->edges_count; i++) {
        edge_t *cur = g->edges[i];
        if (cur->start == e->start && cur->end == e->end) return 0;
    }
    edge_t **new = &g->edges[g->edges_count++];
    *new = (edge_t*) malloc(sizeof(edge_t));
    if (*new == NULL) return t_err("malloc");
    (*new)->start = e->start;
    (*new)->end = e->end;
    add_vertex(g, e->start);
    add_vertex(g, e->end);
    return 0;
}

void free_graph(graph_t *g) {
    for (int i = 0; i < g->edges_count; i++) {
        free(g->edges[i]);
    }
    if (g->edges != NULL) free(g->edges);
    if (g->vertices != NULL) free(g->vertices);
}

void shuffle(int list[], int size) {
    for (int i = size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = list[i];
        list[i] = list[j];
        list[j] = temp;
    }
}

int is_after(edge_t *e, int *order, int count) {
    for (int i = 0; i < count; i++) {
        if (order[i] == e->start) return 0;
        if (order[i] == e->end) return 1;
    }
    return -1;
}
