#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <cstdlib>

int main() {
    FILE *in = fopen("in.txt", "r");

    int n;
    fscanf(in, "%d", &n);
    int *arr = (int*) malloc(sizeof(int) * n);


    FILE *out = fopen("out.txt", "w");
    for (int i = 0; i < n; ++i) {
        fscanf(in, "%d", &arr[i]);
        if (i != 0) {
            if (arr[i - 1] > arr[i]) {
                fprintf(out, "not sorted\ni = %d\narr[i] = %d, arr[i + 1] = %d", i - 1, arr[i - 1], arr[i]);
                return 0;
            }
        }
    }
    fclose(in);
    
    fprintf(out, "sorted");
    fclose(out);

    return 0;
}