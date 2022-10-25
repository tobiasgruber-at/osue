typedef struct Edge {
    int start;
    int end;
} edge_t;

typedef struct Graph {
    struct Edge **edges;
    int *vertices;
    int edges_count, vertices_count;
} graph_t;

/**
 * @brief Adds an edge and its' vertices to a graph.
 * @details Only adds if it does not already exist.
 */
void add_edge(graph_t *g, int start, int end);

/** @brief Prints a graph. */
void print_graph(graph_t *g);

/** Frees all allocated memory of a graph. */
void free_graph(graph_t *g);

/**
 * @brief Shuffles a list of numbers.
 * @details Makes use of the Fisher-Yates algorithm.
 */
void shuffle(int src[], int size);

/**
 * Checks edge (u,v) fulfills that index of u > index of v for a given order.
 * @param order Order of the vertices.
 * @param count Count of the vertices.
 * @param e Edge to be checked.
 * @return 0 if u > v, 1 if u \<= v, -1 if order of both not specified.
 */
int is_after(int *order, int count, edge_t *e);