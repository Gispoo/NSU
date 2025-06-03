#include <stdio.h>

void hello_from_static_lib() {
    printf("Hello from static library!\n");
}

// gcc -c static_lib.c -o static_lib.o
// ar rcs libstatic.a static_lib.o
