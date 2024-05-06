#define _CRT_SECURE_NO_WARNINGS
#include <cstdlib>
#include <stdio.h>
#include <math.h>

int my_min(int a, int b) {
    if (a >= b) {
        return b;
    }
    else {
        return a;
    }
}

int main() {
    int n, a = 1;
    scanf("%d", &n);
    int *arr1 = (int*) malloc(sizeof(int) * n);
    int *arr2 = (int*) malloc(sizeof(int) * n);

    for (int i = 0; i < 2 * n; ++i) {
        if (i < n) {
            scanf("%d", &arr1[i]);
        }
        else {
            scanf("%d", &arr2[i - n]);
        }
    }

    for (int i = 0; i < n; ++i) {
        if (arr1[0] == arr2[i]) {
            for (int j = 1; j < n; ++j) {
                if (i + j >= n) {
                    if (arr1[j] == arr2[i + j - n]) {
                        ++a;
                    }
                }
                else {
                    if (arr1[j] == arr2[i + j]) {
                        ++a;
                    }
                }
            }
        }
        if (a == n) {
            printf("Yes. Cycle shift = %d", my_min(i, n - i));
            return 0;
        }
    }

    printf("No, a = %d", a);

    return 0;
}