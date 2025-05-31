#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b) {
    int c = *a;
    *a = *b;
    *b = c;
}


void QuickSort(int *arr, int n) {
    int sup_el = n - 1, j = -1; 
    for (int i = 0; i < n; ++i) {
        if (arr[i] <= arr[sup_el]) {
            ++j;
            if (j < i) {
                swap(&arr[i], &arr[j]);
            }
        }
    }

    if (j > 1) {
        QuickSort(&arr[0], j);
    }
    if (n - j > 1) {
        QuickSort(&arr[j], n - j);
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


    QuickSort(&arr[0], n);

    
    for (int i = 0; i < n; ++i) {
        printf("%d ", arr[i]);
    }

    free(arr);

    return 0;
}