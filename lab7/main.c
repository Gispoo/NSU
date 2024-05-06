#define _SRC_SECURE_NO_WARNINGS
#include "TopSort/top_sort.h"
#include "Errors/errors.h"
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE* in = fopen("in.txt", "r");
    if (in == NULL) {
        error_open_file();
    }
    FILE* out = fopen("out.txt", "w");
    if (out == NULL) {
        fclose(in);
        error_open_file();
    }

    top_sort(in, out);

    fclose(in);
    fclose(out);

    return 0;
}
