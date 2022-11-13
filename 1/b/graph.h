/**
 * Graph module definitions.
 * @brief Covers all necessary types and operations on graphs.
 * @details Provides operations to add edges to a graph, free it's resources or shuffle its vertices.
 * @file graph.h
 * @author Tobias Gruber, 11912367
 * @date 23.10.2022
 **/

#include "misc.h"

/** Edge of a graph. */
typedef struct Edge {
    int start; /**< Start vertex. */
    int end; /**< End vertex. */
} edge_t;

/** Graph containing vertices and edges. */
typedef struct Graph {
    struct Edge **edges; /**< List of edges that are referencing vertices by their value. */
    int *vertices; /**< List of vertices, represented as integers. */
    int edges_count; /** Number of edges. */
    int vertices_count; /** Number of vertices. */
} graph_t;

/**
 * @brief Adds an edge and its' vertices to a graph.
 * @details Only adds the edge if it wasn't already added.
 * @param g Pointer to the graph.
 * @param e Pointer to the edge that should be added.
 * @return 0 on success, -1 on error.
 */
int add_edge(graph_t *g, edge_t *e);

/**
 * @brief Frees all allocated memory of a graph.
 * @details Frees the allocated memory for each edge its lists of edges and vertices.<br>
 * Lists are only freed if they were allocated.
 * @param g Pointer to the graph.
 */
void free_graph(graph_t *g);

/**
 * @brief Shuffles a list of integers.
 * @details Shuffling is implemented by means of the Fisher-Yates algorithm.
 * @see https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
 * @param list Array to shuffle. Will also be updated with the shuffled values.
 * @param size Size of the list.
 */
void shuffle(int list[], int size);

/**
 * @brief Checks if two vertices respect a given order.
 * @details Checks if an edge (u, v) fulfills that index u > index of v for a given order.
 * @param e Pointer to edge that will be checked.
 * @param order List representing the order of the vertices.
 * @param count Count of the vertices in the list.
 * @return 0 if u > v, 1 if u \<= v, -1 if order of both not specified.
 */
int is_after(edge_t *e, int *order, int count);
