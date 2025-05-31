#include "KMP/kmp.h"

#include <stdio.h>

int main() {
    FILE* in = fopen("in.txt", "r");
    FILE* out = fopen("out.txt", "w");

    if (in == NULL) {
        printf("Error open in.");
        return 0;
    }
    if (out == NULL) {
        fclose(in);
        printf("Error open out.");
        return 0;
    }

    kmp(in, out);

    fclose(in);
    fclose(out);

    return 0;
}
