#define _CRT_SECURE_NO_WARNINGS
#define MAX_SIZE 17
#define MAX_SIZE2 256
#include <stdio.h>
#include <string.h>

void stop_table(char *pat, int *d, int len_p) {
    for (int i = 0; i < MAX_SIZE2; ++i) {
        d[i] = len_p;
    }

    for (int i = len_p - 2; i >= 0; --i) {
        if (d[(unsigned char) pat[i]] == len_p) {
            d[(unsigned char) pat[i]] = len_p - i - 1;
        }
    } 
}

int search_boyer_moore(char *txt, char *pat, int index_in_txt, int *d, int len_t, int len_p) {
    if (len_t < len_p) {
        return 0;
    }

    int index_in_str = 0;
    while (index_in_str <= len_t - len_p) {
        for (int j = len_p - 1; j >= 0; --j) {
            printf("%d ", index_in_txt + index_in_str + j + 1);
            
            if (pat[j] != txt[index_in_str + j]) {
                break;
            }
        }
        index_in_str += d[(unsigned char) txt[index_in_str + len_p - 1]];
    }

    return index_in_str;
}

int main() {
    FILE *in = fopen("in.txt", "r");
    if (in == NULL) {
        return 0;
    }

    char pat[MAX_SIZE + 1];
    if (fgets(pat, MAX_SIZE + 1, in) == NULL) {
        fclose(in);
        return 0;
    }
    int len_p = strlen(pat);
    pat[len_p - 1] = '\0';

    int d[MAX_SIZE2] = {0};
    stop_table(pat, d, len_p - 1);

    char txt[MAX_SIZE2 + 1] = {'\0'};
    int number_of_characters_read;
    int index_in_txt = 0;

    while ((number_of_characters_read = fread(txt, sizeof(char), MAX_SIZE2, in)) > 0) {
        int len_t = strlen(txt);
        int c = search_boyer_moore(txt, pat, index_in_txt, d, len_t, len_p - 1);
        index_in_txt += c;

        if (number_of_characters_read == MAX_SIZE2) {
            for (int i = 0; i < MAX_SIZE2 - c; ++i) {
                txt[i] = txt[MAX_SIZE2 - c + i];
            }

            for (int i = MAX_SIZE2 - c; i < MAX_SIZE2; ++i) {
                txt[i] = '\0';
            }
        }
    }

    fclose(in);
    return 0;
}