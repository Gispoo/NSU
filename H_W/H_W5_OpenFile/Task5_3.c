#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main() {
    FILE *in = fopen("in.txt", "r");
    char a;
	int symbols[256] = {0}, number_symbols = 0;
	
	

	for ( ; !feof(in); ) {
		fscanf(in, "%c", &a);
		++symbols[a];
		++number_symbols;
	}

	printf("%d\n", number_symbols - 1);
	for (int i = 0; i < 256; ++i) {
		if (symbols[i] != 0) {
			printf("%c - %d\n", i, symbols[i]);
		}
	}

    fclose(in);

    
    return 0;
}