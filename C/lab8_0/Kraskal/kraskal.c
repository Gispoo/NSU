#include "kraskal.h"
#include "../Errors/errors.h"

#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int a;
    int b;
    int len;
} Edge;

typedef struct {
    int parent;
    int rank;
} Vertic;

int compare(const void* a, const void* b) {
    Edge* edge_a = (Edge*) a;
    Edge* edge_b = (Edge*) b;

    if (edge_a->len < edge_b->len) return -1;

    if (edge_a->len > edge_b->len) return 1;

    return 0;
}

int find_parent(int v, Vertic mass[]) {
    if (v != mass[v].parent)
        return find_parent(mass[v].parent, mass);

    return mass[v].parent;
}

void asd(Vertic mass[], int parentA, int parentB, int* counter) {
    ++(*counter);
    if (mass[parentA].rank < mass[parentB].rank) {
        mass[parentA].parent = parentB;
    } else if (mass[parentA].rank > mass[parentB].rank) {
        mass[parentB].parent = parentA;
    } else {
        mass[parentB].parent = parentA;
        ++(mass[parentA].rank);
    }
}

void unite(Vertic mass[], Edge* edge, int* counter) {
    int parentA = find_parent(edge->a - 1, mass);
    int parentB = find_parent(edge->b - 1, mass);

    if (parentA == parentB) {
        edge->len = 0;
        return;
    } else {
        asd(mass, parentA, parentB, counter);
    }
}

void enter(int num_vertic, int num_edg, Edge matrix_edge[]) {
    if (num_vertic < 0 || num_vertic > MAX_SIZE)
        bad_number_of_vertices();

    if (num_edg < 0 || num_edg > num_vertic * (num_vertic - 1) / 2)
        bad_number_of_edges();

    for (int i = 0; i < num_edg; ++i) {
        int a, b;
        long long len;
        if (scanf("%d %d %lld", &a, &b, &len) != 3)
            bad_number_of_lines();

        if (a < 1 || a > num_vertic || b < 1 || b > num_vertic)
            bad_vertex();

        if (len < 0 || len > INT_MAX)
            bad_length();

        matrix_edge[i].a = a;
        matrix_edge[i].b = b;
        matrix_edge[i].len = len;
    }
}

void kraskal() {
    int num_vertic, num_edg;
    if (scanf("%d %d", &num_vertic, &num_edg) != 2)
        error_read();

    Edge* matrix_edge = (Edge*) malloc(num_edg * (sizeof(Edge)));
    
    enter(num_vertic, num_edg, matrix_edge);

    qsort(matrix_edge, num_edg, sizeof(Edge), compare);

    Vertic mass[MAX_SIZE];
    for (int i = 0; i < num_vertic; ++i)
        mass[i].parent = i;

    int counter = 0;
    for (int i = 0; i < num_edg; ++i)
        unite(mass, &(matrix_edge[i]), &counter);

    if (counter != num_vertic - 1)
        no_spanning_tree();

    for (int i = 0; i < num_edg; ++i) {
        if (matrix_edge[i].len != 0)
            printf("%d %d\n", matrix_edge[i].a, matrix_edge[i].b);
    }

    free(matrix_edge);
}