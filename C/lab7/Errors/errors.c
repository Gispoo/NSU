#include "errors.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE1 20
#define MAX_SIZE2 100

enum error_codes {
    ERROR_OPEN_FILE = 1,
    ERROR_READ,
    BAD_NUMBER_OF_VERTICES,
    BAD_NUMBER_OF_EDGES,
    BAD_VERTEX,
    BAD_NUMBER_OF_LINES,
    IMPOSSIBLE_TO_SORT
};

static char errors_name[MAX_SIZE1][MAX_SIZE2] = {
    "Error open file.",
    "Error read.",
    "bad number of vertices",
    "bad number of edges",
    "bad vertex",
    "bad number of lines",
    "impossible to sort"
};

void error_open_file() {
    printf("%s", errors_name[ERROR_OPEN_FILE - 1]);
    exit(0);
}

void error_read(FILE* in, FILE* out) {
    printf("%s", errors_name[ERROR_READ - 1]);
    fclose(in);
    fclose(out);
    exit(0);
}

void bad_number_of_vertices(FILE* in, FILE* out) {
    printf("%s", errors_name[BAD_NUMBER_OF_VERTICES - 1]);
    fclose(in);
    fclose(out);
    exit(0);
}

void bad_number_of_edges(FILE* in, FILE* out) {
    printf("%s", errors_name[BAD_NUMBER_OF_EDGES - 1]);
    fclose(in);
    fclose(out);
    exit(0);
}

void bad_vertex(FILE* in, FILE* out) {
    printf("%s", errors_name[BAD_VERTEX - 1]);
    fclose(in);
    fclose(out);
    exit(0);
}

void bad_number_of_lines(FILE* in, FILE* out) {
    printf("%s", errors_name[BAD_NUMBER_OF_LINES - 1]);
    fclose(in);
    fclose(out);
    exit(0);
}

void impossible_sort(FILE* in, FILE* out) {
    printf("%s", errors_name[IMPOSSIBLE_TO_SORT - 1]);
    fclose(in);
    fclose(out);
    exit(0);
}
