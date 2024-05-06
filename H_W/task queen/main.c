#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#define NOT_SET 0

int check_queen_pos(int queen_pos[], int row, int column) {
    for (int j = row - 1; j >= 0; --j) {
        if (queen_pos[j] == column || queen_pos[j] == column + j || queen_pos[j] == column - j) {
            return 0;
        }
    }
    return 1;
}

void queen_problem(int queen_pos[], int current_line, int table_size) {
    if (current_line == table_size) {
        print_board(queen_pos, table_size);
    }

    for (int i = 0; i < table_size; ++i) {
        if (check_queen_pos(queen_pos, current_line, i)) {
            queen_pos[current_line] = i;
            queen_problem(queen_pos, current_line + 1, table_size);
            queen_pos[current_line] = NOT_SET;
        }
    }
}

void print_board() {

}

int main() {

    return 0;
}