#define _SRC_SECURE_NO_WARNINGS

#include "Errors/errors.h"
#include "Dijkstra/dijkstra.h"

#include <stdio.h>

int main() {
    FILE* in = fopen("in.txt", "r");
    FILE* out = fopen("out.txt", "w");

    if (in == NULL || out == NULL) error_open_file();

    dijkstra(in, out);

    fclose(in);
    fclose(out);

    return 0;
}

