#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

void stats_symbols_Task6(char *line) {
	char a;
	int symbols[256] = {0};
	scanf("%c", &line[0]);
	

	for (int i = 1; line[i - 1] != '.'; ++i) {
		if (i == 4096) {
			printf("Error");
			break;
		}
		else {
			scanf("%c", &line[i]);
			a = line[i - 1];
			++symbols[a];
		}
	}

	for (int i = 0; i < 256; ++i) {
		if (symbols[i] != 0) {
			printf("%c - %d\n", i, symbols[i]);
		}
	}
}

int main() {
	char line[4096];
	stats_symbols_Task6(line);
}