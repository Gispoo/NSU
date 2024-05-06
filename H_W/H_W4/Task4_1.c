#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <cctype>
#include <locale.h>
#include <wchar.h>
#include <windows.h>


int main() {
    setlocale (LC_ALL, "Russian");
    SetConsoleOutputCP (1251);
    SetConsoleCP (1251);

    char arr[4096];
    int digit = 0, space = 0, alpha = 0, punct = 0;
    scanf("%c", &arr[0]);

    for (int i = 0; arr[i] != '\n'; ++i) {
        scanf("%c", &arr[i + 1]);
        if (isdigit(arr[i]) != 0) {
            ++digit;
        }
        else if (isspace(arr[i]) != 0) {
            ++space;
        }
        else if (isalpha(arr[i]) != 0) {
            ++alpha;
        }
        else if (ispunct(arr[i]) != 0) {
            ++punct;
        }
    }

    printf("Digit - %d, space - %d, alpha - %d, punct - %d", digit, space, alpha, punct);
    
    
    return 0;
}