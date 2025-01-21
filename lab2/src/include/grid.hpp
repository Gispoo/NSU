#ifndef GRID_H
#define GRID_H

#include "./nonogram.hpp"
#include <vector>
#include <raylib.h>

class Grid {
public:
    // Конструктор
    Grid(const std::string& filePath, int cellSize, int screenWidth, int screenHeight);

    void DrawCross(int x, int y, int size, Color color) const;

    // Рисует сетку на экране
    void draw() const;

    void drawClue() const;

    // Устанавливает состояние ячейки
    void setStateCell(int row, int col, int state);

    // Возвращает состояние ячейки
    int getStateCell(int row, int col) const;

    // Возвращает индексы ячейки, на которую нажал пользователь, по координатам мыши
    bool getCellFromMousePos(int mouseX, int mouseY, int &row, int &col) const;

    // Сбрасывает сетку к начальному состоянию
    void resetGrid();
    
    Nonogram userPuzzle; // Кроссворд игрока

private:
    int cellSize;      // Размер ячейки
    int leftBordGrid;
    int upBordGrid;
    std::vector<std::vector<int>> cells; // Состояние ячеек
    int fontSize = 30;
};

#endif
