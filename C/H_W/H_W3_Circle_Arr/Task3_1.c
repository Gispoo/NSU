#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <locale.h>

int main() {
    setlocale(LC_ALL, "Rus");
    
    int n, increase = 0, decreasing = 0;
    scanf("%d", &n);
    int *arr = (int*) malloc(n * sizeof(int));

    for (int i = 0; i < n; ++i) {
        scanf("%d", &arr[i]);
        if (i > 0) {
            if (arr[i] >= arr[i - 1]) {
                ++increase;
            }
            if (arr[i] <= arr[i - 1]) {
                ++decreasing;
            }
        }
    }

    if (increase == n - 1) {
        printf("-->");
    }
    else if (decreasing == n - 1) {
        printf("<--");
    }
    else {
        printf("--");
    }


    return 0;
}