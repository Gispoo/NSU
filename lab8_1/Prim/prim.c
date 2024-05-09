#include "../Errors/errors.h"

#include <stdio.h>
#include <stdlib.h>

#define INT_MAX 2147483647
#define MAX_SIZE 5000

void close(FILE* in, FILE* out) {
    fclose(in);
    fclose(out);
}

void enter(FILE* in, FILE* out, int n, int m, int* graf) {
    if (n < 0 || n > MAX_SIZE) {
        close(in, out);
        bad_number_of_vertices();
    }

    if (m < 0 || m > n * (n - 1) / 2) {
        close(in, out);
        bad_number_of_edges();
    }

    for (int i = 0; i < m; ++i) {
        int a, b, len;
        if (fscanf(in, "%d %d %d", &a, &b, &len) != 3) {
            close(in, out);
            error_read();
        }

        if (a < 1 || a > n || b < 1 || b > m) {
            close(in, out);
            bad_vertex();
        }

        if (len < 0 || len > INT_MAX) {
            close(in, out);
            bad_length();
        }

        graf[n * (a - 1) + b - 1] = len;
    }
}

void prim(FILE* in, FILE* out) {
    int n, m;
    if (fscanf(in, "%d %d", &n, &m) != 2) {
        close(in, out);
        error_read();
    }

    int* graf = (int*) malloc(n * n * sizeof(int));

    enter(in, out, n, m, graf);

    
}