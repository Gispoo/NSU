#include "./include/game.hpp"
#include "raylib.h"
#include <iostream>

Game::Game(const std::string& filePath, int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight),
    level1("nonograms/Snowflake.txt"), level2("nonograms/Rook.txt"),
    isGameRunning(true),
    grid(filePath, 30, screenWidth, screenHeight) {}


void Game::runGame() {
    while (isGameRunning) {
        // Обработка ввода
        handleInput();

        // Проверка решения
        checkSolution();

        // Отрисовка
        draw();
        
        if (WindowShouldClose()) {
            isGameRunning = false;
        }
    }
}

void Game::draw() const {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Отрисовка сетки
    grid.draw();

    // Отрисовка подсказок
    grid.drawClue();

    // Отрисовка сообщения о победе
    // if (isSolved()) {
    //     DrawText("You Win!", );
    // }

    EndDrawing();
}

void Game::handleInput() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        int row, col;
        if(grid.getCellFromMousePos((int)mousePos.x, (int)mousePos.y, row, col)) {
            grid.setStateCell(row, col, (grid.getStateCell(row, col) + 1) % 2);
        }
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 mousePos = GetMousePosition();
        int row, col; 
        if(grid.getCellFromMousePos((int)mousePos.x, (int)mousePos.y, row, col)) {
            int currentState = grid.getStateCell(row, col);
            int newState;

            if(currentState == 0 || currentState == 1) { // Белая или черная -> серая
                newState = 2;
            } else { // Серая -> белая
                newState = 0;
            }
            grid.setStateCell(row, col, newState);
        }
    }
}

void Game::resetGame() {
    grid.resetGrid(); // Сбрасываем сетку
}

void Game::checkSolution() {
    int rows = grid.userPuzzle.rows;
    int cols = grid.userPuzzle.cols;
    std::vector<std::vector<int>> userGrid(rows, std::vector<int>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            userGrid[i][j] = grid.getStateCell(i, j);
        }
    }

    if(grid.userPuzzle.isSolved(userGrid)) {
        std::cout << "Solved!" << std::endl; // TODO: Implement proper win screen
        isGameRunning = false;
    } else {
            //std::cout << "Not solved" << std::endl;
    }
}

