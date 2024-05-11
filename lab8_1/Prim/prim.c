#include "../Errors/errors.h"

#include <stdio.h>
#include <stdlib.h>

#define INT_MAX 2147483647
#define MAX_SIZE 5000

int enter(FILE* in, int n, int m, int graf[]) {
    if (n < 0 || n > MAX_SIZE)
        return bad_number_of_vertices();

    if (m < 0 || m > n * (n - 1) / 2)
        return bad_number_of_edges();

    for (int i = 0; i < m; ++i) {
        int a, b;
        long long len;
        if (fscanf(in, "%d %d %lld", &a, &b, &len) != 3)
            return bad_number_of_lines();

        if (a < 1 || a > n || b < 1 || b > n)
            return bad_vertex();

        if (len < 0 || len > INT_MAX)
            return bad_length();

        graf[n * (a - 1) + b - 1] = len - 1;
        graf[n * (b - 1) + a - 1] = len - 1;
    }
    return 0;
}

void free_all(int* key, int* parent, int* graf) {
    free(key);
    free(parent);
    free(graf);
}

void update_key(int graf[], int new_v, int key[], int n) {
    key[new_v] = -1;
    for (int j = 0; j < n; ++j) {
        if (graf[new_v * n + j] != -1 && graf[new_v * n + j] < key[j]) 
            key[j] = graf[new_v * n + j];
    }
}

int min_key(int key[], int* min_w, int n) {
    int index = 0;
    for (int i = 0; i < n; ++i) {
        if (key[i] != -1 && key[i] < *min_w) {
            *min_w = key[i];
            index = i;
        }
    }
    return index;
}

int get_vertex_by_weight(int graf[], int new_v, int weigth, int n) {
    for (int i = 0; i < n; ++i) {
        if (graf[new_v * n + i] == weigth) return i;
    }
    return -1;
}

void print_answer(FILE* out, int parent[], int n) {
    for (int i = 1; i < n; ++i) {
        if (parent[i] == -1) {
            no_spanning_tree();
            return;
        }
    }

    for (int i = 1; i < n; ++i) {
        fprintf(out, "%d %d\n", i + 1, parent[i] + 1);
    }

    return;
}

void init(int parent[], int key[], int graf[], int n) {
    for (int i = 0; i < n; ++i) {
        parent[i] = -1;
        key[i] = INT_MAX;
        for (int j = 0; j < n; ++j) {
            graf[n * i + j] = -1;
        }
    }
}
 
void prim(FILE* in, FILE* out) {
    int n, m;
    if (fscanf(in, "%d %d", &n, &m) != 2) {
        error_read();
        return;
    }

    if (n == 0) {
        no_spanning_tree();
        return;
    }

    int* graf = (int*) malloc(n * n * sizeof(int));
    int* parent = (int*) malloc(n * sizeof(int));
    int* key = (int*) malloc(n * sizeof(int));

    init(parent, key, graf, n);
    
    if (enter(in, n, m, graf)) {
        free_all(key, parent, graf);
        return;
    }

    update_key(graf, 0, key, n);

    for (int count = 1; count < n; ++count) {
        int min_w = INT_MAX;
        int new_v = min_key(key, &min_w, n);

        parent[new_v] = get_vertex_by_weight(graf, new_v, min_w, n);

        update_key(graf, new_v, key, n);
    }

    print_answer(out, parent, n);

    free_all(key, parent, graf);
}
