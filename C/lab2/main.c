#define _CRT_SECURE_NO_WARNINGS
#define MAX_SIZE 11
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char* next_permutation(char* permutation, int len) {
    int first_position = 0;
    for (int i = len - 2; i >= 0; --i) {
        if (permutation[i] < permutation[i + 1]) {
            first_position = i;
            break;
        } else if (i == 0) {
            return NULL;
        }
    }
    
    int min_index = first_position + 1;
    for (int j = min_index + 1; j < len; ++j) {
        if (permutation[j] > permutation[first_position] && permutation[j] < permutation[min_index]) {
            min_index = j;
        }
    }
    
    char temp = permutation[first_position];
    permutation[first_position] = permutation[min_index];
    permutation[min_index] = temp;
    
    for (int j = first_position + 1; j < len - 1; ++j) {
        for (int k = j + 1; k < len; ++k) {
            if (permutation[j] > permutation[k]) {
                temp = permutation[j];
                permutation[j] = permutation[k];
                permutation[k] = temp;
            }
        }
    }

    return permutation;
}

int checking_conditions(char *arr, int len) {
    int number_of_digit[256] = {0};
    for (int i = 0; i < len; ++i) {
        if (isdigit(arr[i]) == 0) {
            printf("bad input\n");
            return 0;
        }
        
        int code;
        code = (int) arr[i];
        ++number_of_digit[code];
        if (number_of_digit[code] > 1) {
            printf("bad input\n");
            return 0;
        }
    }
    return 1;
}

int main() {
    char permutation[MAX_SIZE + 1];
    int n;

    if (gets(permutation) == NULL) {
        return 0;
    }
    if (scanf("%d", &n) != 1) {
        return 0;
    }

    int len = strlen(permutation);
    
    if (checking_conditions(permutation, len) == 0) {
        return 0;
    }

    char* next;
    for (int i = 1; (next = next_permutation(permutation, len)) != NULL && i <= n && len > 1; ++i) {
        printf("%s\n", next);
    }

    return 0;
}