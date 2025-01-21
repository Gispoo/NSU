#ifndef GRID_H
#define GRID_H

#include "./nonogram.hpp"
#include <vector>
#include <raylib.h>

class Grid {
public:
    // �����������
    Grid(const std::string& filePath, int cellSize, int screenWidth, int screenHeight);

    void DrawCross(int x, int y, int size, Color color) const;

    // ������ ����� �� ������
    void draw() const;

    void drawClue() const;

    // ������������� ��������� ������
    void setStateCell(int row, int col, int state);

    // ���������� ��������� ������
    int getStateCell(int row, int col) const;

    // ���������� ������� ������, �� ������� ����� ������������, �� ����������� ����
    bool getCellFromMousePos(int mouseX, int mouseY, int &row, int &col) const;

    // ���������� ����� � ���������� ���������
    void resetGrid();
    
    Nonogram userPuzzle; // ��������� ������

private:
    int cellSize;      // ������ ������
    int leftBordGrid;
    int upBordGrid;
    std::vector<std::vector<int>> cells; // ��������� �����
    int fontSize = 30;
};

#endif
