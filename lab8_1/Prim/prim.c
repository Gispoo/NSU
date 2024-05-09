#include "../Errors/errors.h"

#include <stdio.h>
#include <stdlib.h>

#define INT_MAX 2147483647
#define MAX_SIZE 5000

void close(FILE* in, FILE* out) {
    fclose(in);
    fclose(out);
}

typedef struct {
    int a;
    int b;
    int len;
} Edge;

void enter(FILE* in, FILE* out, int n, int m, Edge edges[]) {
    if (n < 0 || n > MAX_SIZE) {
        close(in, out);
        bad_number_of_vertices();
    }

    if (m < 0 || m > n * (n - 1) / 2) {
        close(in, out);
        bad_number_of_edges();
    }

    for (int i = 0; i < m; ++i) {
        if (fscanf(in, "%d %d %d", &edges[i].a, &edges[i].b, &edges[i].len) != 3) {
            close(in, out);
            error_read();
        }

        if (edges[i].a < 1 || edges[i].a > n || edges[i].b < 1 || edges[i].b > m) {
            close(in, out);
            bad_vertex();
        }

        if (edges[i].len < 0 || edges[i].len > INT_MAX) {
            close(in, out);
            bad_length();
        }
    }
}

void prim(FILE* in, FILE* out) {
    int n, m;
    if (fscanf(in, "%d %d", &n, &m) != 2) {
        close(in, out);
        error_read();
    }

    Edge* edges = (Edge*) malloc(n * (sizeof(Edge)));

    enter(in, out, n, m, edges);


}