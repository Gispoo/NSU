#define _CRT_SECURE_NOWARNINGS
#include <stdio.h>
#include <ctype.h>
#include <string.h>

void normal_space(char *str) {
    for (int i = 0; i < strlen(str) - 1; ++i) {
        if (str[i] == ' ' && str[i + 1] == ' ') {
            for (int j = 1; j + i < strlen(str) - 1; ++j) {
                str[i + j] = str[i + j + 1];
                if (i + j == strlen(str) - 2) {
                    str[i + j + 1] = '\0';
                }
            }
            --i;
        }
    }
}

int main() {
    char str[4096];
    scanf("%c", &str[0]);
    for (int i = 0; str[i] != '\n'; ++i) {
        scanf("%c", &str[i + 1]);
    }
    
    normal_space(str);

    printf("%s", str);

    return 0;
}