#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>


int summa(int mass[], int n) {
	int sum = 0;
	for (int i = 0; i < n; ++i) {
		sum += mass[i];
	}
	return sum;
}

int minimum(int mass[], int n) {
	int min = 10000;
	for (int i = 0; i < n; ++i) {
		if (mass[i] < min) {
			min = mass[i];
		}
	}
	return min;
}

int maximum(int mass[], int n) {
	int max = -10000;
	for (int i = 0; i < n; ++i) {
		if (mass[i] > max) {
			max = mass[i];
		}
	}
	return max;
}

int num_min(int mass[], int n, int min) {
	int n_m = 0;
	for (int i = 0; i < n; ++i) {
		if (mass[i] == min) {
			++n_m;
		}
	}
	return n_m;
}

int Task_5() {
	int n;
	scanf("%d", &n);
	if (n > 4096) {
		printf("Error");
		return 0;
	}

	int mass[1000];
	for (int i = 0; i < n; ++i) {
		scanf("%d", &mass[i]);
	}

	int min = minimum(mass, n), a;
	while (1) {
		printf("Enter number task - ");
		scanf("%d", &a);
		if (a == 1) {
			printf("%d", summa(mass, n));
			return 0;
		}
		else if (a == 2) {
			printf("%d", minimum(mass, n));
			return 0;
		}
		else if (a == 3) {
			printf("%d", maximum(mass, n));
			return 0;
		}
		else if (a == 4) {
			printf("%d", num_min(mass, n, min));
			return 0;
		}
		else {
			printf("That task is not\n");
		}
	}
}