#include "graph.h"
#include "path.h"
#include "stack.h"
#include "vertices.h"

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define OPTIONS     "hvui:o:"
#define BUFFERSIZE  100
#define CITYNAMELEN 64

// Variables for DFS Function
bool verbose_flag = false;
int recur_call_count = 0;
int tab_len = 0;
uint32_t recursive_calls = 0;

// DFS Function
// Conducts Depth-first Search to find paths
void dfs(Graph *G, uint32_t v, Path *current, Path *shortest, char *cities[], FILE *outfile) {

    // More variables
    recur_call_count++;
    uint32_t tempRd;
    uint32_t weight;
    int len = graph_vertices(G);
    bool baseCond = true;
    path_push_vertex(current, v, G);

    // Checks weight of <v, w> and if vertex was visited
    for (int w = 0; w < len; w++) {
        weight = graph_edge_weight(G, v, w);
        if ((weight != 0) && !graph_visited(G, w)) {
            baseCond = false;
            break;
        }
    }

    // Checks if all vertices were visited (goes into loop if true)
    if (baseCond) {
        // Gets first vertex from current path
        if (path_vertices(current) == graph_vertices(G)) {
            Path *tempPath = path_create();
            bool retStat;
            uint32_t rdNodeValue;
            path_copy(tempPath, current);

            // Parsing through temporary path of all vertices
            while (path_vertices(tempPath)) {
                retStat = path_pop_vertex(tempPath, &rdNodeValue, G);
            }

            // Frees memory of temporary path
            path_delete(&tempPath);

            // Adds final vertex
            path_push_vertex(current, rdNodeValue, G);

            // Checks -v condition
            if (verbose_flag) {
                path_print(current, outfile, cities);
            }

            // Checks shortest vs current path
            if ((path_length(shortest) == 0) || (path_length(current) < path_length(shortest))) {
                path_copy(shortest, current);
            }

            // Removes extra vertex
            path_pop_vertex(current, &rdNodeValue, G);
        }
    }

    // Checks visited vertices
    graph_mark_visited(G, v);

    // Goes through all vertices of graph
    for (uint32_t w = 0; w < graph_vertices(G); w++) {
        weight = graph_edge_weight(G, v, w);

        if (weight == 0) {
            continue;
        }
        if (graph_visited(G, w)) {
            continue;
        }

        // Recursive call
        recursive_calls++;
        dfs(G, w, current, shortest, cities, outfile);

        graph_mark_unvisited(G, w);
        path_pop_vertex(current, &tempRd, G);
    }
}

// Own strdup() Function
// Removes extra newline character
int fgetscopy(char *dst, char *src) {
    char ch;
    while ((ch = *src)) {
        if ((ch == '\n') || (ch == '\r')) {
            *dst = 0;
            break;
        }
        *dst = *src;
        dst++;
        src++;
    }
    *dst = 0;
    return 0;
}

// Main Function
int main(int argc, char **argv) {

    // Initializes necessary variables
    int opt = 0;
    bool undirected = false;
    FILE *fp = stdin;
    FILE *outfp = stdout;
    char buffer[BUFFERSIZE];
    char *retptr;
    int vertexcount = 0;
    char *cities_ar = NULL;
    char **cities = NULL;
    uint32_t node_i, node_j, node_k;
    Graph *gptr;
    Path *current_path;
    Path *shortest_path;

    // getopt function
    // Checks command line options
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'h':
            // Prints help message
            printf("SYNOPSIS\n  Traveling Salesman Problem using DFS.\n\n");
            printf("USAGE\n  ./tsp [-u] [-v] [-h] [-i infile] [-o outfile]\n\n");
            printf("OPTIONS\n");
            printf("  -u             Use undirected graph.\n");
            printf("  -v             Enable verbose printing.\n");
            printf("  -h             Program usage and help.\n");
            printf("  -i infile      Input containing graph (default: stdin)\n");
            printf("  -o outfile     Output of computed path (default: stdout)\n");
            break;
        case 'v':
            // Prints all paths
            verbose_flag = true;
            break;
        case 'u':
            // Specifies graph to be undirected
            undirected = true;
            break;
        case 'i':
            // Specifies input file
            fp = fopen(optarg, "r");
            if (fp == NULL) {
                printf("error opening file %s\n", optarg);
                goto errorexit;
            }
            break;
        case 'o':
            // Specifies output file
            outfp = fopen(optarg, "w");
            if (outfp == NULL) {
                printf("error file write %s\n", optarg);
                goto errorexit;
            }
            break;
        }
    }

    // Allocates needed memory to support cities
    retptr = fgets(buffer, BUFFERSIZE, fp);
    sscanf(buffer, "%d", &vertexcount);
    cities_ar = malloc(CITYNAMELEN * vertexcount);

    if (vertexcount < 1 || vertexcount > VERTICES) {
        printf("invalid vertices\n");
        goto errorexit;
    }

    // Sets up cities and allocates needed memory
    cities = malloc(sizeof(char *) * vertexcount);
    for (int i = 0; i < vertexcount; i++) {
        retptr = fgets(buffer, BUFFERSIZE, fp);
        if (retptr == NULL) {
            goto errorexit;
        }
        fgetscopy(&cities_ar[i * CITYNAMELEN], buffer);
        cities[i] = &cities_ar[i * CITYNAMELEN];
    }

    // Creates graph and checks conditions for i, j, and k
    gptr = graph_create(vertexcount, undirected);
    while (1) {
        retptr = fgets(buffer, BUFFERSIZE, fp);
        if (retptr == NULL) {
            break;
        }
        sscanf(buffer, "%u %u %u", &node_i, &node_j, &node_k);
        if (node_i > VERTICES || node_j > VERTICES) {
            printf("invalid matrix values\n");
            goto errorexit;
        }
        graph_add_edge(gptr, node_i, node_j, node_k);
    }

    // Creates paths for current and shortest
    current_path = path_create();
    shortest_path = path_create();

    // Calls DFS and prints path information
    dfs(gptr, START_VERTEX, current_path, shortest_path, cities, outfp);
    path_print(shortest_path, outfp, cities);
    printf("Total recursive calls: %u \n", recursive_calls + 1);

    //Frees memory
    graph_delete(&gptr);
    path_delete(&current_path);
    path_delete(&shortest_path);

// Closes file if error present and frees pointers
errorexit:
    if (fp != stdin) {
        fclose(fp);
    }
    if (outfp != stdout) {
        fclose(outfp);
    }
    if (cities != NULL) {
        free(cities);
    }
    if (cities_ar != NULL) {
        free(cities_ar);
    }
    return 0;
}
