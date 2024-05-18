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

void next_node(int parent[], int s, int n_v) {
    if (n_v == s - 1) return;

    int next = s - 1;
    while (parent[next] != -1) {
        next = parent[next];
    }
    parent[next] = n_v;
}

void update_key(int graf[], Data* state, int n_v, int s, int f, int n, int* n_p) {
    for (int j = 0; j < n; ++j) {
        if (graf[n_v * n + j] != -1 && graf[n_v * n + j] + state->key[n_v] <= state->key[j]) {
            if (j == f - 1) ++(*n_p);

            next_node(state->parent, s, n_v);
            
            state->key[j] = graf[n_v * n + j] + state->key[n_v];
        }
    }
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

int min_key(Data* state, long long* min_w, int n) {
    int index = -1;
    for (int i = 0; i < n; ++i) {
        if (state->parent[i] == -2 && state->key[i] < *min_w) {
            *min_w = state->key[i];
            index = i;
        }
    }
    return index;
}

void print_first_line(FILE* out, long long key[], int n) {
    for (int i = 0; i < n; ++i) {
        if (key[i] == LONG_LONG_MAX) fprintf(out, "oo ");

        else if (key[i] > INT_MAX) fprintf(out, "INT_MAX+ ");
        
        else fprintf(out, "%d ", key[i]);
    }

    fprintf(out, "\n");
}

void print_second_line(FILE* out, Data* state, int s, int f, int n, int n_p) {
    if (s == f) {
        fprintf(out, "%d", s);
        return;
    }

    if (n_p == 0) {
        fprintf(out, "no path");
        return;
    }

    if (state->key[f - 1] > INT_MAX && n_p > 1) {
        fprintf(out, "overflow");
        return;
    }

    int next = s - 1, i = 0;
    int* answer = (int*) calloc(n, sizeof(int));
    for ( ; state->parent[next] != -1; ++i) {
        answer[i] = next + 1;
        next = state->parent[next];
    }
    answer[i] = f;
    
    for (i = n - 1; i >= 0; --i) {
        if (answer[i] != 0) fprintf(out, "%d ", answer[i]);
    }
    free(answer);

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
        free(graf);
        return;
    }

    int n_p = 0;
    update_key(graf, &state, s - 1, s, f, n, &n_p);

    for (int count = 1; count < n; ++count) {
        long long min_w = LONG_LONG_MAX;
        int n_v = min_key(&state, &min_w, n);

        if (n_v == -1) break;
        state.parent[n_v] = -1;

        update_key(graf, &state, n_v, s, f, n, &n_p);

        if (n_v == f - 1) next_node(state.parent, s, n_v);
    }

    print_first_line(out, state.key, n);
    print_second_line(out, &state, s, f, n, n_p);

    free_all(&state, graf);
}