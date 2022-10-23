#include "graph.h"
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Adds a vertex to a graph.
 * @details Only adds if it does not already exist.
 */
static void add_vertex(graph_t *g, int idx) {
    vertex_t *cur = g->v_head;
    for (; cur; cur = cur->next) {
        if (cur->idx == idx) return;
    }
    vertex_t *prev = g->v_head;
    g->v_head = malloc(sizeof(vertex_t));
    g->v_head->idx = idx;
    g->v_head->next = prev;
}

void add_edge(graph_t *g, int start, int end) {
    edge_t *cur = g->e_head;
    for (; cur; cur = cur->next) {
        if (cur->start == start && cur->end == end) return;
    }
    edge_t *prev = g->e_head;
    g->e_head = malloc(sizeof(edge_t));
    g->e_head->start = start;
    g->e_head->end = end;
    g->e_head->next = prev;
    add_vertex(g, start);
    add_vertex(g, end);
}

void print(graph_t *g) {
    vertex_t *cur_v = g->v_head;
    printf("Vertices: ");
    for (; cur_v; cur_v = cur_v->next) {
        printf("%i, ", cur_v->idx);
    }
    edge_t *cur_e = g->e_head;
    printf("\nEdges: ");
    for (; cur_e; cur_e = cur_e->next) {
        printf("(%i-%i), ", cur_e->start, cur_e->end);
    }
    printf("\n");
}

void free_graph(graph_t *g) {
    while(g->v_head) {
        vertex_t *next = g->v_head->next;
        free(g->v_head);
        g->v_head = next;
    }
    while(g->e_head) {
        edge_t *next = g->e_head->next;
        free(g->e_head);
        g->e_head = next;
    }
}
