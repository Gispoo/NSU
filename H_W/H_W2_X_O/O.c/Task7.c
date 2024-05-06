#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>

void swap_char(char *arr1, int i, int end) {
	char c = arr1[i];
	arr1[i] = arr1[end];
	arr1[end] = c;
}


void Task_7() {
	char arr1[256], arr2[256];

	scanf("%s", &arr1);
	scanf("%s", &arr2);


	for (int i = 0; i < (strlen(arr1)) / 2; ++i) {
		swap_char(arr1, i, strlen(arr1) - i - 1);
	}

	for (int i = 0; i < (strlen(arr2)) / 2; ++i) {
		swap_char(arr2, i, strlen(arr2) - i - 1);
	}


	printf("%s%s", arr2, arr1);

}
