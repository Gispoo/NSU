#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

int main() {
    int n, k;
    FILE *in = fopen("D:\\VS code\\H_W5\\in.txt", "r");

    fscanf(in, "%d%d", &k, &n);

    int *arr1 = (int*) malloc(sizeof(int) * n);
    int *arr2 = (int*) calloc(k, sizeof(int));


    for (int i = 0; i < n; ++i) {
        fscanf(in, "%d", &arr1[i]);
        ++arr2[arr1[i]];
    }
    fclose(in);


    FILE *out = fopen("D:\\VS code\\H_W5\\out.txt", "w");
    for (int i = 0; i < k; ++i) {
        if (arr2[i] > 0) {
            for (int j = 0; j < arr2[i]; ++j) {
                fprintf(out, "%d", i);
            }
        }
    }
    fclose(out);
  

    return 0;
}