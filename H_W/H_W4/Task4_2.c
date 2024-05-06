#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int strlen(char *str) {
    int len = 0;
    
    for ( ; str[len] != '\n'; ++len) {
    }

    return len;
}


int strcmp(char *str1, char *str2) {
    for (int i = 0; i < strlen(str1); ++i) {
        if ((int) str1[i] > (int) str2[i] || strlen(str1) > strlen(str2)) {
            return 1;
        }
        else if ((int) str1[i] < (int) str2[i] || strlen(str1) < strlen(str2)) {
            return -1;
        }
    }
    return 0;
}

char* strcpy(char *arr1, char *arr2) {
    for (int i = 0; i < strlen(arr1); ++i) {
        arr2[i] = arr1[i];
    }
    return arr2;
}


int main() { 
    char str[4096];
    scanf("%c", &str[0]);

    for (int i = 0; str[i] != '\n'; ++i) {
        scanf("%c", &str[i + 1]);
    }
    
    int number_f;
    char space;
    printf("1 - strlen\n2 - strcmp\n3 - strcpy\n");
    scanf("%d%c", &number_f, &space);

    if (number_f == 1) {
        printf("len str = %d\n", strlen(str));
    }
    
    else if (number_f == 2) {
        char str2[4096];
        scanf("%c", &str2[0]);
        for (int i = 0; str2[i] != '\n'; ++i) {
           scanf("%c", &str2[i + 1]);
        }
        if (strcmp(str, str2) == 0) {
            printf("str1 = str2\n");
        }
        else if (strcmp(str, str2) == -1) {
            printf("str1 < str2\n");
        }
        else {
            printf("str1 > str2\n");
        }
    }

    else {
        char arr2[4096];
        printf("str' = %s\n", strcpy(str, arr2));
    }


    return 0;
}
