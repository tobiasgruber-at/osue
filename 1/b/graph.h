#include "misc.h"

/** @brief Edge of a graph. */
typedef struct Edge {
    int start; /**< Number of the start vertex. */
    int end; /**< Number of the end vertex. */
} edge_t;

/** @brief Graph containing vertices and edges. */
typedef struct Graph {
    struct Edge **edges; /**< List of edges that are referencing vertices by their value. */
    int *vertices; /**< List of vertices, represented as integers. */
    int edges_count; /** Number of edges. */
    int vertices_count; /** Number of vertices. */
} graph_t;

/**
 * @brief Adds an edge and its' vertices to a graph.
 * @details Only adds if it does not already exist.
 * @param g Pointer to the graph.
 * @param e Pointer to the edge that should be added.
 * @return 0 on success, -1 on error.
 */
int add_edge(graph_t *g, edge_t *e);

/**
 * Frees all allocated memory of a graph.
 * @param g Pointer to the graph.
 */
void free_graph(graph_t *g);

/**
 * @brief Shuffles a list of numbers.
 * @details Shuffle is implemented with the Fisher-Yates algorithm.
 * @param list List to shuffle. Will be updated with the shuffled values.
 * @param size Size of the list.
 */
void shuffle(int list[], int size);

/**
 * @brief Checks if two vertices respect a given order.
 * @details Checks if an edge (u,v) fulfills that index of u > index of v for a given order.
 * @param order Order of the vertices.
 * @param count Count of the vertices.
 * @param e Edge to be checked.
 * @return 0 if u > v, 1 if u \<= v, -1 if order of both not specified.
 */
int is_after(int *order, int count, edge_t *e);
