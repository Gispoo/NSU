#ifndef LAB5_ERRORS_H
#define LAB5_ERRORS_H

#include <stdio.h>
#include "../huffman.h"
#define NUM_LINES 20

enum error_codes {
    OPEN_FILE = 1,
    READ_SYMBOL,
    ROOT_NULL,
    ERROR_ACTION_IN_NEXT_INDEX
};

static char error_names[NUM_LINES][MAX_SIZE] = {
    "Error open file",
    "Error reading a symbol",
    "Root == NULL",
    "Error action in next index"
};

void error_open_file();

void error_read_symbol();

void root_null();

void error_action_in_next_index();

#endif