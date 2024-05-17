#define _SRC_SECURE_NO_WARNINGS

#include "../Errors/errors.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 5000
#define SHIFT -2

void free_all(int* key, int* parent, int* graf) {
    free(key);
    free(parent);
    free(graf);
}

int chek_condition(int n, int m, int start, int end) {
    if (n < 0 || n > MAX_SIZE)
        return bad_number_of_vertices();
    
    if (m < 0 || m > n * (n - 1) / 2)
        return bad_number_of_edges();

    if (start < 1 || start > n || end < 1 || end > n)
        return bad_vertex();

    return 0;
}

int enter(FILE* in, int n, int m, int graf[]) {
    for (int i = 0; i < m; ++i) {
        int a, b;
        long long len;
        if (fscanf(in, "%d %d %lld", &a, &b, &len) != 3)
            return bad_number_of_lines();

        if (a < 1 || a > n || b < 1 || b > n)
            return bad_vertex();

        if (len < 0 || len > INT_MAX)
            return bad_length();

        graf[n * (a - 1) + b - 1] = len + SHIFT;
        graf[n * (b - 1) + a - 1] = len + SHIFT;
    }
    return 0;
}

void next_node(int parent[], int start, int new_v) {
    int next = start - 1;
    while (parent[next] != -1) {
        next = parent[next];
    }
    parent[next] = new_v;
}

void update_key(int graf[], int key[], int parent[], int new_v, int start, int end, int n) {
    for (int j = 0; j < n; ++j) {
        if (graf[new_v * n + j] != SHIFT - 1 && (long long) graf[new_v * n + j] + key[new_v] < key[j]) {
            if (j == end - 1)
                next_node(parent, start, new_v);
            
            if ((long long) graf[new_v * n + j] + key[new_v] > INT_MAX + 2 * SHIFT) {
                key[j] = INT_MAX - 1;
            } else {
                key[j] = graf[new_v * n + j] + key[new_v];
            }
        }
    }
}

void init(int graf[], int key[], int parent[], int start, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            graf[i * n + j] = SHIFT - 1;
        }
        key[i] = INT_MAX;
        parent[i] = -2;
    }

    key[start - 1] = 0;
    parent[start - 1] = -1;
}

int min_key(int parent[], int key[], int* min_w, int n) {
    int index = 0;
    for (int i = 0; i < n; ++i) {
        if (parent[i] == -2 && key[i] < *min_w) {
            *min_w = key[i];
            index = i;
        }
    }
    return index;
}

void print_answer(FILE* out, int parent[], int key[], int start, int end, int n) {
    for (int i = 0; i < n; ++i) {
        if (key[i] == INT_MAX) fprintf(out, "oo ");

        else if (key[i] == INT_MAX - 1) fprintf(out, "INT_MAX+ ");
        
        else if (i == start - 1) fprintf(out, "0 ");
        
        else fprintf(out, "%d ", key[i] - SHIFT);
    }

    for (int i = 1; i < n; ++i) {
        if (parent[i] == -2) {
            fprintf(out, "\nno path");
            return;
        }
    }

    long long len_path = 0;
    int next = start - 1;
    while (parent[next] != -1) {
        len_path += key[parent[next]];
        next = parent[next];
    }

    if (len_path > INT_MAX) fprintf(out, "\noverflow");

    for (int i = 1; i < n; ++i) {
        fprintf(out, "%d ", parent[i]);
    }

    return;
}

void dijkstra(FILE* in, FILE* out) {
    int n, m, start, end;
    if (fscanf(in, "%d %d %d %d", &n, &start, &end, &m) != 4) {
        error_read();
        return;
    }

    if (chek_condition(n, m, start, end)) return;

    int* graf = (int*) malloc(n * n * sizeof(int));
    int* key = (int*) malloc(n * sizeof(int));
    int* parent = (int*) malloc(n * sizeof(int));

    init(graf, key, parent, start, n);

    if (enter(in, n, m, graf)) {
        free(graf);
        return;
    }

    update_key(graf, key, parent, start - 1, start, end, n);
    
    for (int count = 1; count < n; ++count) {
        int min_w = INT_MAX;
        int new_v = min_key(parent, key, &min_w, n);

        parent[new_v] = -1;

        update_key(graf, key, parent, new_v, start, end, n);
    }

    print_answer(out, parent, key, start, end, n);

    free_all(key, parent, graf);
}