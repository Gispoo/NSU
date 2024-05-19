#include "stdio.h"
#include "string.h"

#define MAX_SIZE 17
#define MAX_SIZE2 20

void get_table_of_shifts(FILE* out, char pattern[], char arr[], int len_p) {
    if (len_p) fprintf(out, "0 ");
    for (int i = 1, j = 0; i < len_p; ) {
        if (pattern[j] == pattern[i]) {
            arr[i] = j + 1;
            fprintf(out, "%d ", j + 1);
            ++i, ++j;
        } else if (j == 0) {
            fprintf(out, "0 ");
            ++i;
        } else {
            j = arr[j - 1];
        }
    }
    fprintf(out, "\n");
}

int read(FILE* in, char txt[]) {
    for (int i = 0; i < MAX_SIZE2; ++i) {
        if (!txt[i]) {
            if ((txt[i] = fgetc(in)) == EOF)
                return 1;
        }
    }
    return 0;
}

void buff(char txt[], int len_txt, int index) {
    for (int j = 0; j < len_txt - index; ++j) {
        txt[j] = txt[index + j];
    }

    for (int i = len_txt - index; i < MAX_SIZE2; ++i) {
        txt[i] = '\0';
    }
}

void search(FILE* in, FILE* out, char pattern[], char arr[], int len_p) {
    char txt[MAX_SIZE2] = {0};
    int position = 0, len_txt = 0;
    
    int end;
    while (1) {
        end = read(in, txt);
        len_txt = strlen(txt);
        int i = 0, j = 0;
        
        while (i - j + len_p < len_txt) {
            if (txt[i] == pattern[j]) {
                ++i, ++j;

                if (j == len_p) {
                    fprintf(out, "%d %d ", i - j + 1 + position, j);
                    j = arr[j - 1];
                }
            } else if (j == 0) {
                ++i;
            } else {
                fprintf(out, "%d %d ", i - j + 1 + position, j);
                j = arr[j - 1];
            }
        } 
        position += i;

        buff(txt, len_txt, i);
        if (end) break;
    }
}

void kmp(FILE* in, FILE* out) {
    char pattern[MAX_SIZE] = {0};
    if (!fgets(pattern, MAX_SIZE + 1, in)) 
        return;
    
    int len_p = strlen(pattern) - 1;
    pattern[len_p] = '\0';
    if (len_p == 0) return;

    char arr[MAX_SIZE] = {0};
    get_table_of_shifts(out, pattern, arr, len_p);

    search(in, out, pattern, arr, len_p);
}