#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <cstdlib>
#include <locale.h>
#include <wchar.h>
#include <windows.h>


int main() {
	setlocale (LC_ALL, "Russian");
	SetConsoleOutputCP (1251);
	SetConsoleCP (1251);


    int n, symbols[256] = {0}, alone = 0, two = 0;
    scanf("%d", &n);
    int *arr = (int*) malloc(sizeof(int) * n);


    for (int i = 0; i < n; ++i) {
        scanf("%d", &arr[i]);
		char code = '0' + arr[i];
		if (arr[i] < 1 || arr[i] > n - 1) {
			printf("Condition 1 not met");
    		return 0;
		}
		++symbols[code];
    }

    
	for (int i = 0; i < 256; ++i) {
		if (symbols[i] == 1) {
			++alone;
		}
		else if (symbols[i] == 2 && two != 1) {
			++two;
			printf("duble - %c\n", i);
		}
	}

	if (alone == n - 2) {
		printf("Condition 1 is met\n");
	}
	else {
		printf("Condition 1 not met\n");
	}


	//printf("????????");

    return 0;
}