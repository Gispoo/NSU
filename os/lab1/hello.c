#include <stdio.h>

void hello_from_static_lib();
// void hello_from_dynamic_lib();

int main() {
    printf("Hello world\n");
    hello_from_static_lib();
    // hello_from_dynamic_lib();
    return 0;
}

// gcc hello.c -o hello -L. -lstatic
// gcc hello.c -o hello -L. -ldynamic -Wl,-rpath=.