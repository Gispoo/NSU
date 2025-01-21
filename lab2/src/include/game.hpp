#ifndef GAME_H
#define GAME_H

#include "./nonogram.hpp"
#include "./grid.hpp"
#include "raylib.h"
#include <string>

class Game {
public:
    // �����������
    Game(const std::string& filePath, int screenWidth, int screenHeight);

    // �������� ������� ����
    void runGame();

    // ��������� �������� ����
    void draw() const;

    // ��������� ����� ������������
    void handleInput();

    // ����� ���� � ���������� ��������� (����� �����)
    void resetGame();

    // �������� ����� �� ���������
    void checkSolution();
    
private:
    Grid grid;               // ����� ��� �������� ����
    bool isGameRunning;      // ����, ��� ���� ��������
    int screenWidth;
    int screenHeight;
    Nonogram level1;         // ������� 1
    Nonogram level2;         // ������� 2
};

#endif

