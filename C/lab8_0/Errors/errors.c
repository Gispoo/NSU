#include <stdio.h>
#include <stdlib.h>
#include "errors.h"
#include "../Kraskal/kraskal.h"

void error_open_file() {
    printf("%s", errors_name[ERROR_OPEN_FILE - 1]);
    exit(0);
}

void error_read() {
    printf("%s", errors_name[ERROR_READ - 1]);
    //close_all(files);
    exit(0);
}

void bad_number_of_vertices() {
    printf("%s", errors_name[ERROR_FIRST_CONDITION - 1]);
    //close_all(files);
    exit(0);
}

void bad_number_of_edges() {
    printf("%s", errors_name[ERROR_SECOND_CONDITION - 1]);
    //close_all(files);
    exit(0);
}

void bad_vertex() {
    printf("%s", errors_name[ERROR_THIRD_CONDITION - 1]);
    //close_all(files);
    exit(0);
}

void bad_length() {
    printf("%s", errors_name[ERROR_FOURTH_CONDITION - 1]);
    //close_all(files);
    exit(0);
}

void bad_number_of_lines() {
    printf("%s", errors_name[ERROR_FIFTH_CONDITION - 1]);
    //close_all(files);
    exit(0);
}

void no_spanning_tree() {
    printf("%s", errors_name[ERROR_SIXTH_CONDITION - 1]);
    //close_all(&files);
    exit(0);
}