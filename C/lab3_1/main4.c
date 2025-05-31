#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b) {
    int c = *a;
    *a = *b;
    *b = c;
}

int partition(int *arr, int left, int right) {
    int i = left, j = right, pivot = arr[(right + left) / 2];

    for ( ; 1; ++i, --j) {
        while (arr[i] < pivot)
            ++i;
        while (arr[j] > pivot)
            --j;
        if (i >= j) {
            return j;
        } 
        swap(&arr[i], &arr[j]);
    }
}

void quick_sort(int arr[], int left, int right) {
    if (left < right) {
        int k = partition(arr, left, right);

        quick_sort(arr, left, k);
        quick_sort(arr, k + 1, right);
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
            return 0;
        }
    }

    quick_sort(&arr[0], 0, n - 1);

    for (int i = 0; i < n; ++i) {
        printf("%d ", arr[i]);
    }

    free(arr);

    return 0;
}