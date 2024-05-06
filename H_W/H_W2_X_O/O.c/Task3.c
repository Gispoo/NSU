#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <math.h>

void Task_3() {
	float a, b, c;
	printf("enter a, b, c = ");
	scanf("%f %f %f", &a, &b, &c);

	float D, x1, x2;
	D = b * b - 4 * a * c;
	if (D >= 0) {
		x1 = (-b + sqrt(D)) / (2 * a);
		x2 = (-b - sqrt(D)) / (2 * a);
		printf("x1 = %f, x2 = %f", x1, x2);
	}
	else {
		x1 = -b / (2 * a);
		x2 = -b / (2 * a);
		printf("x1 = %f + %fi, x2 = %f - %fi", x1, sqrt(-D) / (2 * a), x2, sqrt(-D) / (2 * a));
	}	
}