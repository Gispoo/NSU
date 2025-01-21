#include "./include/grid.hpp"
#include "raylib.h"

Grid::Grid(const std::string& filePath, int cellSize, int screenWidth, int screenHeight) :
    userPuzzle(filePath), 
    cellSize(cellSize), 
    cells(userPuzzle.rows, std::vector<int>(userPuzzle.cols, 0)),
    leftBordGrid(screenWidth / 2 - cellSize * userPuzzle.cols / 2),
    upBordGrid(screenHeight / 2 - cellSize * userPuzzle.rows / 2) {}


void Grid::DrawCross(int x, int y, int size, Color color) const {
    int halfSize = size / 2;

    // Рисуем первую диагональную линию
    DrawLine(x - halfSize, y - halfSize, x + halfSize, y + halfSize, color);

    // Рисуем вторую диагональную линию
    DrawLine(x + halfSize, y - halfSize, x - halfSize, y + halfSize, color);
}

void Grid::draw() const {
    for (int i = 0; i < userPuzzle.rows; ++i) {
        for (int j = 0; j < userPuzzle.cols; ++j) {
            int x = j * cellSize + leftBordGrid;
            int y = i * cellSize + upBordGrid;
            
            Color cellColor;

            switch (cells[i][j]) {
            case 0:
                cellColor = WHITE;
                break;
            case 1:
                cellColor = BLACK;
                break;
            default:
                DrawCross(x + cellSize / 2, y + cellSize / 2, cellSize, GRAY);
                DrawRectangleLines(x, y, cellSize, cellSize, DARKGRAY);
                continue;
                break;
            }
            DrawRectangle(x, y, cellSize, cellSize, cellColor);
            DrawRectangleLines(x, y, cellSize, cellSize, DARKGRAY);
        }
    }
}

void Grid::drawClue() const {
    for (size_t i = 0; i < userPuzzle.rowClues.size(); ++i) {
        int numClues = userPuzzle.rowClues[i].size();
        int yPos = upBordGrid + i * cellSize + 2;

        std::string clueString;
        for(size_t j = 0; j < numClues; ++j) {
            clueString += std::to_string(userPuzzle.rowClues[i][j]) + " ";
        }

        int textWidth = MeasureText(clueString.c_str(), fontSize);
        int xPos = leftBordGrid - textWidth;

        DrawText(clueString.c_str(), xPos, yPos, fontSize, BLACK);
    }
    for (int i = 0; i < userPuzzle.cols; ++i) {
        int numClues = userPuzzle.colClues[i].size();
        int xPos = leftBordGrid + (i + 0.35) * cellSize;

        std::string clueString;
        for(int j = 0; j < numClues; ++j) {
            clueString += std::to_string(userPuzzle.colClues[i][j]) + "\n";
        }

        int yPos = upBordGrid - fontSize * numClues - 2;

        DrawText(clueString.c_str(), xPos, yPos, fontSize, BLACK);
    }    
}

void Grid::setStateCell(int row, int col, int state) {
    if(row >= 0 && row < userPuzzle.rows && col >= 0 && col < userPuzzle.cols) {
        cells[row][col] = state;
    }
}

int Grid::getStateCell(int row, int col) const {
    if(row >= 0 && row < userPuzzle.rows && col >= 0 && col < userPuzzle.cols) {
        return cells[row][col];
    }
    return -1;
}

bool Grid::getCellFromMousePos(int mouseX, int mouseY, int &row, int &col) const {
    bool xInside = mouseX >= leftBordGrid && mouseX < leftBordGrid + userPuzzle.cols * cellSize;
    bool yInside = mouseY >= upBordGrid && mouseY < upBordGrid + userPuzzle.rows * cellSize;

    if(xInside && yInside) {
        col = (mouseX - leftBordGrid) / cellSize;
        row = (mouseY - upBordGrid) / cellSize;
        return true;
    }
    return false;
}

void Grid::resetGrid() {
    for(int i = 0; i < userPuzzle.rows; ++i){
        for(int j = 0; j < userPuzzle.cols; ++j){
            cells[i][j] = 0;
        }
    }
}