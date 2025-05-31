#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 30

enum {
    ERROR_OPEN_FILE = 1,
    ERROR_READ,
    BAD_NUMBER_OF_VERTICES,
    BAD_NUMBER_OF_EDGES,
    BAD_VERTEX,
    BAD_LENGHT,
    BAD_NUMBER_OF_LINES,
    NO_SPANNING_TREE
};

char errors[MAX_SIZE][MAX_SIZE] = {
    "Error open file.",
    "Error read.",
    "bad number of vertices",
    "bad number of edges",
    "bad vertex",
    "bad length",
    "bad number of lines",
    "no spanning tree"
};

void error_open_file() {
    printf("%s\n", errors[ERROR_OPEN_FILE - 1]);
    exit(0);
}

void error_read() {
    printf("%s\n", errors[ERROR_READ - 1]);
    return;
}

int bad_number_of_vertices() {
    printf("%s\n", errors[BAD_NUMBER_OF_VERTICES - 1]);
    return 1;
}

int bad_number_of_edges() {
    printf("%s\n", errors[BAD_NUMBER_OF_EDGES - 1]);
    return 1;
}

int bad_vertex() {
    printf("%s\n", errors[BAD_VERTEX - 1]);
    return 1;
}

int bad_length() {
    printf("%s\n", errors[BAD_LENGHT - 1]);
    return 1;
}

int bad_number_of_lines() {
    printf("%s\n", errors[BAD_NUMBER_OF_LINES - 1]);
    return 1;
}

void no_spanning_tree() {
    printf("%s\n", errors[NO_SPANNING_TREE - 1]);
    return;
}
