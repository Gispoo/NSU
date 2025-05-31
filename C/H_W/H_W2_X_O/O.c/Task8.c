#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int user1(char *desk) {
	int x, y;
	for ( ; 1; ) {
		scanf("%d %d", &y, &x);
		if (desk[(x - 1) * 8 + 2 * y - 1] == ' ') {
			desk[(x - 1) * 8 + 2 * y - 1] = 'X';
			return 0;
		}
		else {
			printf("Эта клетка занята");
		}
	}
}

int user2(char *desk) {
	int x, y;
	for ( ; 1; ) {
		scanf("%d %d", &x, &y);
		if (desk[(x - 1) * 8 + 2 * y - 1] == ' ') {
			desk[(x - 1) * 8 + 2 * y - 1] = 'O';
			return 0;
		}
		else {
			printf("Эта клетка занята");
		}
	}
}

int program(char *desk) {
	for ( ; 1; ) {
		int x = rand() % 3;
		int y = rand() % 3;
		if (desk[(x) * 8 + 2 * (y + 1) - 1] == ' ') {
			desk[(x) * 8 + 2 * (y + 1) - 1] = 'O';
			return 0;
		}
	}
}

void desk_now(char *desk) {
	for (int i = 0; i < 3; ++i) {
		for (int j = 0; j < 7; ++j) {
			printf("%c", desk[i * 8 + j]);
		}
		printf("\n");
	}
	printf("\n");
}

    
int end_game(char *desk) {
	if ((desk[1] == desk[3] && desk[3] == desk[5] && desk[5] == 'X') || (desk[9] == desk[11] && desk[11] == desk[13] && desk[13] == 'X') || (desk[17] == desk[19] && desk[19] == desk[21] && desk[21] == 'X')) {
		return 1;
	}
	else if ((desk[1] == desk[3] && desk[3] == desk[5] && desk[5] == 'O') || (desk[9] == desk[11] && desk[11] == desk[13] && desk[13] == 'O') || (desk[17] == desk[19] && desk[19] == desk[21] && desk[21] == 'O')) {
		return 2;
	}
	else if ((desk[1] == desk[9] && desk[9] == desk[17] && desk[17] == 'X') || (desk[3] == desk[11] && desk[11] == desk[19] && desk[19] == 'X') || (desk[5] == desk[13] && desk[13] == desk[21] && desk[21] == 'X')) {
		return 1;
	}
	else if ((desk[1] == desk[9] && desk[9] == desk[17] && desk[17] == 'O') || (desk[3] == desk[11] && desk[11] == desk[19] && desk[19] == 'O') || (desk[5] == desk[13] && desk[13] == desk[21] && desk[21] == 'O')) {
		return 2;
	}
	else if ((desk[1] == desk[11] && desk[11] == desk[21] && desk[21] == 'X') || (desk[5] == desk[11] && desk[11] == desk[17] && desk[17] == 'X')) {
		return 1;
	}
	else if ((desk[1] == desk[11] && desk[11] == desk[21] && desk[21] == 'O') || (desk[5] == desk[11] && desk[11] == desk[17] && desk[17] == 'O')) {
		return 2;
	}
	else {
		return 0;
	}
}


int main() {
	char desk[] = {"| | | | | | | | | | | |"};


	for ( ; end_game(desk) != 1; ) {
		user1(desk);

		if (end_game(desk) == 1) {
			printf("User win!");
			return 0;
		}

		desk_now(desk);


		program(desk);

		if (end_game(desk) == 2) {
			printf("Program win!");
			return 0;
		}

		desk_now(desk);
	}	

}
