#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>
#include <locale.h>

void Task_2() {
	setlocale(LC_ALL, "Rus");

	int a = 0, n;
	printf("enter n = ");
	scanf("%d", &n);

	for (int i = 2; i < n; ++i) {
		for (int j = 2; j <= sqrt(i); ++j) {
			if (i % j == 0) {
				++a;
			}
		}
		if (a == 0) {
			printf("%d\n", i);
		}
		a = 0;
	}
}