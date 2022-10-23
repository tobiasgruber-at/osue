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
int shuffle(int src[], int size);