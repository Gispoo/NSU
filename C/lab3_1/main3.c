#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>



void swap(int *a, int *b) {
    int c = *a;
    *a = *b;
    *b = c;
}


void QuickSort(int *arr, int left, int right) {
    int i = left, j = right, x = arr[(left + right) / 2];
    do {
        while (arr[i] < x) {
            ++i;
        }
        while (x < arr[j]) {
            --j;
        }
        if (i <= j) {
            swap(&arr[i], &arr[j]);
            ++i;
            --j;
        }
    } while (i < j);
    
    if (left < j) {
        QuickSort(arr, left, j);
    }
    if (i < right) {
        QuickSort(arr, i, right);
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


    QuickSort(&arr[0], 0, n - 1);

    
    for (int i = 0; i < n; ++i) {
        printf("%d ", arr[i]);
    }

    free(arr);


    return 0;
}