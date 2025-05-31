#define _CRT_SECUDE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

int bracket_balance(char *str) {
    char bracket[4096] = {0};

    for (int i = 0, j = 0; i < strlen(str); ++i) {
        if (str[i] == '(' || str[i] == '[' || str[i] == '{' || str[i] == '<') {
            bracket[j] = str[i];
            ++j;
        }
        
        else if (str[i] == ')' || str[i] == ']' || str[i] == '}' || str[i] == '>') {
            if (j == 0) {
                return 0;
            }
            else if ((int) bracket[j - 1] + 1 == (int) str[i] || (int) bracket[j - 1] + 2 == (int) str[i]) {
                bracket[j - 1] = '\0';
                --j;
            }
            else {
                return 0;
            }
        }
    }

    if (bracket[0] == '\0') {
        return 1;
    }
    else {
        return 0;
    }
}



int main() {
    char str[4096];
    scanf("%c", &str[0]);
    for (int i = 0; str[i] != '\n'; ++i) {
        scanf("%c", &str[i + 1]);
    }

    if (bracket_balance(str) == 1) {
        printf("bracket balance true\n");
    }
    else {
        printf("bracket balance false\n");
    }

    return 0;
}