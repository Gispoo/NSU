#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>



void swap(int *a, int *b) {
    int c = *a;
    *a = *b;
    *b = c;
}


int Partition(int *arr, int left, int right) {
    int k = 0;
    for ( ; left < right; ) {
        if (arr[left] <= arr[right]) {
            if (k % 2 == 0) {
                --right;
            }
            else {
                ++left;
            }
        }
        else {
            swap(&arr[left], &arr[right]);
            if (k % 2 == 0) {
                ++left;
            }
            else {
                --right;
            }
            ++k;
        }
    }
    return left;
}


void QuickSort(int *arr, int left, int right) {
    if (left < right) {
        int pivot = Partition(arr, left, right);
        QuickSort(arr, left, pivot);
        QuickSort(arr, pivot + 1, right);
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