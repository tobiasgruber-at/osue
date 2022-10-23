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
 * @brief Adds an edge and its' vertices to a graph.
 * @details Only adds if it does not already exist.
 */
void add_edge(graph_t *g, int start, int end);

/** @brief Prints a graph. */
void print(graph_t *g);

/** Frees all allocated memory of a graph. */
void free_graph(graph_t *g);
