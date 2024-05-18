#define _SRC_SECURE_NO_WARNINGS

#include "../Errors/errors.h"

#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 5000

typedef struct {
    long long* key;
    int* parent;
} Data;

void free_all(Data* state, int* graf) {
    free(state->key);
    free(state->parent);
    free(graf);
}

int chek_condition(int n, int m, int s, int f) {
    if (n < 0 || n > MAX_SIZE)
        return bad_number_of_vertices();
    
    if (m < 0 || m > n * (n - 1) / 2)
        return bad_number_of_edges();

    if (s < 1 || s > n || f < 1 || f > n)
        return bad_vertex();

    return 0;
}

void init(int graf[], Data* state, int s, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            graf[i * n + j] = -1;
        }
        state->key[i] = LONG_LONG_MAX;
        state->parent[i] = -2;
    }

    state->key[s - 1] = 0;
    state->parent[s - 1] = -1;
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

        graf[n * (a - 1) + b - 1] = len;
        graf[n * (b - 1) + a - 1] = len;
    }
    return 0;
}

int min_key(Data* state, int n) {
    int index = -1;
    long long min_w = LONG_LONG_MAX;
    for (int i = 0; i < n; ++i) {
        if (state->parent[i] == -2 && state->key[i] < min_w) {
            min_w = state->key[i];
            index = i;
        }
    }
    return index;
}

int get_parent(int graf[], Data* state, int new_v, int n) {
    long long min = LONG_LONG_MAX;
    int index = -1;
    for (int i = 0; i < n; ++i) {
        if (state->parent[i] != -2 && graf[i * n + new_v] != -1) {
            if (state->key[i] + graf[i * n + new_v] < min) {
                min = state->key[i] + graf[i * n + new_v];
                index = i;
            }
        }
    }
    return index;
}

void update_key(int graf[], Data* state, int new_v, int f, int n, int* n_path) {
    for (int j = 0; j < n; ++j) {
        if (graf[new_v * n + j] != -1 && graf[new_v * n + j] + state->key[new_v] <= state->key[j]) {
            if (j == f - 1) ++(*n_path);
            
            state->key[j] = graf[new_v * n + j] + state->key[new_v];
        }
    }
}

void go(Data* state, int graf[], int f, int* n_path, int n) {
    for (int count = 1; count < n; ++count) {
        int new_v = min_key(state, n);

        if (new_v == -1) break;
        state->parent[new_v] = get_parent(graf, state, new_v, n);

        update_key(graf, state, new_v, f, n, n_path);
    }
}

void print_first_line(FILE* out, long long key[], int n) {
    for (int i = 0; i < n; ++i) {
        if (key[i] == LONG_LONG_MAX) fprintf(out, "oo ");

        else if (key[i] > INT_MAX) fprintf(out, "INT_MAX+ ");
        
        else fprintf(out, "%lld ", key[i]);
    }

    fprintf(out, "\n");
}

void print_second_line(FILE* out, Data* state, int s, int f, int n_path) {
    if (s == f) {
        fprintf(out, "%d", s);
        return;
    }

    if (n_path == 0) {
        fprintf(out, "no path");
        return;
    }

    if (state->key[f - 1] > INT_MAX && n_path > 1) {
        fprintf(out, "overflow");
        return;
    }

    int next = f - 1;
    while (state->parent[next] != -1) {
        fprintf(out, "%d ", next + 1);
        next = state->parent[next];
    }
    fprintf(out, "%d ", next + 1);

    return;
}

void dijkstra(FILE* in, FILE* out) {
    int n, m, s, f;
    if (fscanf(in, "%d %d %d %d", &n, &s, &f, &m) != 4) {
        error_read();
        return;
    }

    if (chek_condition(n, m, s, f)) return;

    int* graf = (int*) malloc(n * n * sizeof(int));
    Data state;
    state.key = (long long*) malloc(n * sizeof(long long));
    state.parent = (int*) malloc(n * sizeof(int));

    init(graf, &state, s, n);

    if (enter(in, n, m, graf)) {
        free_all(&state, graf);
        return;
    }

    int n_path = 0;
    update_key(graf, &state, s - 1, f, n, &n_path);

    go(&state, graf, f, &n_path, n);

    print_first_line(out, state.key, n);
    print_second_line(out, &state, s, f, n_path);

    free_all(&state, graf);
}