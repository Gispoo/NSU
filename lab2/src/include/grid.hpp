#ifndef GRID_H
#define GRID_H

#include "./raylib.h"
#include "./nonogram.hpp"
#include "./config.h"
#include <vector>

class Grid {
public:
    Grid(int rows, int cols);
    Grid() = default;

    void draw_cross(int x, int y, int size, Color color) const;
    void draw() const;
    void draw_clue(std::vector<std::vector<int>> row_clues, std::vector<std::vector<int>> colClues) const;
    void set_state_cell(int row, int col, int state);
    int get_state_cell(int row, int col) const;
    bool get_cell_from_mouse_pos(int mouseX, int mouseY, int &row, int &col) const;

private:
    int rows;
    int cols;
    int cell_size;
    int left_bord_grid;
    int up_bord_grid;
    std::vector<std::vector<int>> state_cells;
    int font_size = FONT_SIZE;
};

#endif