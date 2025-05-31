#define CRT_SECURE_NO_WARNINGS
#include <stdio.h>


void print_binary(unsigned int x) {
    int size = sizeof(int) * 8;
    for (int i = 0; i < size; ++i) {
        if (x & (1 << size - i - 1)) {
            printf("1");
        }
        else {
            printf("0");
        }
    }
}