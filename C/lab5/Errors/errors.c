#include <stdio.h>
#include <stdlib.h>
#include "errors.h"

void error_open_file() {
    printf("%s\n", error_names[OPEN_FILE - 1]);
    exit(OPEN_FILE);
}

void error_read_symbol() {
    printf("%s\n", error_names[READ_SYMBOL - 1]);
    exit(READ_SYMBOL);
}

void root_null() {
    printf("%s\n", error_names[ROOT_NULL - 1]);
    exit(ROOT_NULL);
}

void error_action_in_next_index() {
    printf("%s\n", error_names[ERROR_ACTION_IN_NEXT_INDEX - 1]);
    exit(ERROR_ACTION_IN_NEXT_INDEX);
}