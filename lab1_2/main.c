#include "KMP/kmp.h"
#include "Errors/errors.h"

#include <stdio.h>

int main(){
    FILE* in = fopen("in.txt", "r");
    FILE* out = fopen("out.txt", "w");

    if (in == NULL || out == NULL) 
        error_open_file();

    kmp(in, out);

    fclose(in);
    fclose(out);

    return 0;
}
