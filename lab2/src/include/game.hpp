#ifndef GAME_H
#define GAME_H

#include "./nonogram.hpp"
#include "./grid.hpp"
#include "raylib.h"
#include <string>

class Game {
public:
    // Конструктор
    Game(const std::string& filePath, int screenWidth, int screenHeight);

    // Основной игровой цикл
    void runGame();

    // Отрисовка игрового поля
    void draw() const;

    // Обработка ввода пользователя
    void handleInput();

    // Сброс игры к начальному состоянию (новая сетка)
    void resetGame();

    // Проверка решен ли кроссворд
    void checkSolution();
    
private:
    Grid grid;               // Сетка для игрового поля
    bool isGameRunning;      // Флаг, что игра запущена
    int screenWidth;
    int screenHeight;
    Nonogram level1;         // Уровень 1
    Nonogram level2;         // Уровень 2
};

#endif

