#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>

void Task_4() {
	setlocale(LC_ALL, "Rus");

	int n, h, m, s;
	printf("enter time =");
	scanf("%d %d %d", &h, &m, &s);
	printf("enter n = ");
	scanf("%d", &n);

	int s1 = n % 60;
	int h1 = n / 3600;
	int m1 = (n - (3600 * h1)) / 60;
	int M = m + m1 + (s + s1) / 60;
	int H = h + h1 + (M / 60);

	printf("hours: %d, minets: %d, seconds: %d", H % 24, M % 60, (s + s1) % 60);
}