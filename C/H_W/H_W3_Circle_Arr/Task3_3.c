#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <cstdlib>

int main() {
    int n, m;
    scanf("%d", &n);

    int *arr = (int*) malloc(sizeof(int) * n);
    for (int i = 0; i < n; ++i) {
        scanf("%d", &arr[i]);
    }

    scanf("%d", &m);

    for (int i = 0; i < n; ++i) {
        if (m == arr[i]) {
            printf("%d", i);
            return 0;
        }
    }

    printf("Not found\n");

    return 0;
}