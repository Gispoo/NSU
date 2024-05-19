#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 30

enum {
    ERROR_OPEN_FILE = 1,
    ERROR_READ,
};

char errors[MAX_SIZE][MAX_SIZE] = {
    "Error open file.",
    "Error read.",
};

void error_open_file() {
    printf("%s\n", errors[ERROR_OPEN_FILE - 1]);
    exit(0);
}

void error_read() {
    printf("%s\n", errors[ERROR_READ - 1]);
    exit(0);
}