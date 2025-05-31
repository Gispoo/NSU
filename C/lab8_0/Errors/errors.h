#ifndef LAB8_0_ERRORS_H
#define LAB8_0_ERRORS_H

#include "../Kraskal/kraskal.h"

#define MAX_SIZE1 20
#define MAX_SIZE2 100

enum error_codes {
    ERROR_OPEN_FILE = 1,
    ERROR_READ,
    ERROR_FIRST_CONDITION,
    ERROR_SECOND_CONDITION,
    ERROR_THIRD_CONDITION,
    ERROR_FOURTH_CONDITION,
    ERROR_FIFTH_CONDITION,
    ERROR_SIXTH_CONDITION
};

static char errors_name[MAX_SIZE1][MAX_SIZE2] = {
    "Error open file.",
    "Error read.",
    "bad number of vertices",
    "bad number of edges",
    "bad vertex",
    "bad length",
    "bad number of lines",
    "no spanning tree"
};

void error_open_file();

void error_read();

void bad_number_of_vertices();

void bad_number_of_edges();

void bad_vertex();

void bad_length();

void bad_number_of_lines();

void no_spanning_tree();

#endif