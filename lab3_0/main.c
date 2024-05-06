#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b) {
    int c = *b;
    *b = *a;
    *a = c;
}

void build_pyramid(int *arr, int n, int i) {
    int dad = i, left_bro = 2 * i + 1, right_bro = 2 * i + 2;

    if (right_bro < n && arr[right_bro] > arr[dad]) {
        dad = right_bro;
    }
    if (left_bro < n && arr[left_bro] > arr[dad]) {
        dad = left_bro;
    }

    if (dad != i) {
        swap(&arr[i], &arr[dad]);

        build_pyramid(arr, n, dad);
    }    
}

void heap_sort(int *arr, int n) {
    for (int i = n / 2; i >= 0; --i) {
        build_pyramid(arr, n, i);
    }

    for (int i = n - 1; i > 0; --i) {
        swap(&arr[0], &arr[i]);

        build_pyramid(arr, i, 0);
    }
}
 
int main() {
    int n;
    if (scanf("%d", &n) != 1) {
        return 0;
    }

    int *arr = (int*) malloc(sizeof(int) * n);
    for (int i = 0; i < n; ++i) {
        if (scanf("%d", &arr[i]) != 1) {
            free(arr);
            return 0;
        }
    }

    heap_sort(arr, n);

    for (int i = 0; i < n; ++i) {
        printf("%d ", arr[i]);
    }

    free(arr);
    
    return 0;
}
