#include "../Errors/errors.h"

#include "stdio.h"
#include "string.h"

#define MAX_SIZE 16

void get_table_of_shifts(FILE* out, char pattern[MAX_SIZE], char arr[MAX_SIZE], int len) {
    fprintf(out, "0 ");
    for (int i = 1, j = 0; i < len; ) {
        if (pattern[j] == pattern[i]) {
            arr[i] = j + 1;
            fprintf(out, "%d ", j + 1);
            ++i;
            ++j;
        } else if (j == 0) {
            fprintf(out, "0 ");
            ++i;
        } else {
            j = arr[j - 1];
        }
    }
}

void search(FILE* in, FILE* out, char pattern[], char arr[]) {

}

void kmp(FILE* in, FILE* out) {
    char pattern[MAX_SIZE] = {0};
    if (!fgets(pattern, MAX_SIZE, in)) 
        error_read();
    
    int len = strlen(pattern);
    pattern[len - 1] = '\0';
    --len;

    char arr[MAX_SIZE] = {0};
    get_table_of_shifts(out, pattern, arr, len);

    search(in, out, pattern, arr);
}