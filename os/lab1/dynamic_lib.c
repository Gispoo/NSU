#include <stdio.h>

void hello_from_dynamic_lib() {
    printf("Hello from dynamic library!\n");
}

// gcc -fPIC -c dynamic_lib.c -o dynamic_lib.o
// gcc -shared dynamic_lib.o -o libdynamic.so
