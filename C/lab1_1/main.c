#define CRT_SECURE_NO_WARNINGS
#define MAX_SIZE 17
#define MAX_SIZE2 100
#include <stdio.h>
#include <string.h>
#include <math.h>

int hash(char *str, int len) {
    int h = 0;
    int pow3 = 1;

    for (int i = 0; i < len; ++i) {
        h += ((unsigned char) str[i] % 3) * pow3;
        pow3 *= 3;
    }

    return h;
}

int compare(char *str1, char *str2, int len, int index) {
    for (int i = 0; i < len; ++i) {
        printf("%d ", index + i);

        if (str1[i] != str2[i]) {
            return 0;
        }
    }

    return 1;
}

int robin_karp(char* pat, char* txt, int len_p, int len_t, int hash_p, int index, int power_of_three) {
    int hash_t = hash(txt, len_p);
    int i;

    for (i = 0; i <= len_t - len_p; ++i) {
        if (hash_t == hash_p) {
            compare(&txt[i], pat, len_p, i + 1 + index);
        }

        hash_t /= 3;
        hash_t += ((unsigned char) txt[len_p + i] % 3) * power_of_three;
    }

    return i + len_p - 1;
}

int main() {
    FILE *in = fopen("in.txt", "r");

    char pat[MAX_SIZE + 1];
    if (fgets(pat, MAX_SIZE + 1, in) == NULL) {
        fclose(in);
        return 0;
    }
    pat[strlen(pat) - 1] = '\0';

    char txt[MAX_SIZE2];
    int index = 0;
    int len_p = strlen(pat);
    int hash_p = hash(pat, len_p);
    printf("%d ", hash_p);
    int power_of_three = pow(3, len_p - 1);
    
    while (fgets(txt, MAX_SIZE2, in) != NULL) {
        int len_t = strlen(txt);
        index += robin_karp(pat, txt, len_p, len_t, hash_p, index, power_of_three);
    }
    
    fclose(in);

    return 0;
}