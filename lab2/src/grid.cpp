#include "raylib.h"
#include "./include/grid.hpp"
#include "./include/config.h"
#include <iostream>
#include <algorithm>
#include <iterator>

Grid::Grid(int rows, int cols) :
    rows(rows),
    cols(cols),
    cell_size(CELL_SIZE), 
    left_bord_grid(SCREEN_WIDTH / 2 - cell_size * cols / 2),
    up_bord_grid(SCREEN_HEIGHT / 2 - cell_size * rows / 2),
    state_cells(rows, std::vector<int>(cols, 0)) {}

void Grid::draw_cross(int x, int y, int size, Color color) const {
    int halfSize = size / 2;

    DrawLine(x - halfSize, y - halfSize, x + halfSize, y + halfSize, color);
    DrawLine(x + halfSize, y - halfSize, x - halfSize, y + halfSize, color);
}

void Grid::draw() const {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            int x = j * cell_size + left_bord_grid;
            int y = i * cell_size + up_bord_grid;
            
            Color cellColor;

            switch (state_cells[i][j]) {
            case 0:
                cellColor = WHITE;
                break;
            case 1:
                cellColor = BLACK;
                break;
            default:
                draw_cross(x + cell_size / 2, y + cell_size / 2, cell_size, GRAY);
                DrawRectangleLines(x, y, cell_size, cell_size, DARKGRAY);
                continue;
                break;
            }
            DrawRectangle(x, y, cell_size, cell_size, cellColor);
            DrawRectangleLines(x, y, cell_size, cell_size, DARKGRAY);
        }
    }
}

void Grid::draw_clue(std::vector<std::vector<int>> row_clues, std::vector<std::vector<int>> col_clues) const {
    for (size_t i = 0; i < row_clues.size(); ++i) {
        int numClues = row_clues[i].size();
        int yPos = up_bord_grid + i * cell_size + 2;

        std::string clueString;
        for(size_t j = 0; j < numClues; ++j) {
            clueString += std::to_string(row_clues[i][j]) + " ";
        }

        int textWidth = MeasureText(clueString.c_str(), font_size);
        int xPos = left_bord_grid - textWidth;

        DrawText(clueString.c_str(), xPos, yPos, font_size, BLACK);
    }
    for (int i = 0; i < cols; ++i) {
        int numClues = col_clues[i].size();
        int xPos = left_bord_grid + (i + 0.35) * cell_size;

        std::string clueString;
        for(int j = 0; j < numClues; ++j) {
            clueString += std::to_string(col_clues[i][j]) + "\n";
        }

        int yPos = up_bord_grid - font_size * numClues - 2;

        DrawText(clueString.c_str(), xPos, yPos, font_size, BLACK);
    }
}

void Grid::set_state_cell(int row, int col, int state) {
    if(row >= 0 && row < rows && col >= 0 && col < cols) {
        state_cells[row][col] = state;
    }
}

int Grid::get_state_cell(int row, int col) const {
    if(row >= 0 && row < rows && col >= 0 && col < cols) {
        return state_cells[row][col];
    }
    return -1;
}

bool Grid::get_cell_from_mouse_pos(int mouseX, int mouseY, int &row, int &col) const {
    bool xInside = mouseX >= left_bord_grid && mouseX < left_bord_grid + cols * cell_size;
    bool yInside = mouseY >= up_bord_grid && mouseY < up_bord_grid + rows * cell_size;

    if(xInside && yInside) {
        col = (mouseX - left_bord_grid) / cell_size;
        row = (mouseY - up_bord_grid) / cell_size;
        return true;
    }
    return false;
}