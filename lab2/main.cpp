#include "raylib.h"
#include "./src/include/menu.hpp"
#include "./src/include/game.hpp"
#include <vector>
#include <string>
#include <iostream>

int main() {
  // Инициализация Raylib
  const int screenWidth = 1200;
  const int screenHeight = 1200;
  InitWindow(screenWidth, screenHeight, "Nonogram Game");
  SetTargetFPS(60);

  // Создание меню
  std::vector<std::string> menuOptions = {"Level 1", "Level 2", "Exit"};
  Menu menu(menuOptions, screenWidth, screenHeight);
  bool inGame = false;
  Game game("nonograms/Snowflake.txt", screenWidth, screenHeight); //Инициализируем Game, чтобы не было проблем, если inGame == false

  // Главный игровой цикл
  int selectedOption = -1;
  while (!WindowShouldClose()) {
    //Если мы не в игре, то отрисовываем меню
    if(!inGame) {
      // Обработка ввода меню
      int result = menu.handleInput();
      if(result != -1) {
        selectedOption = result;
      }
    }
    // Обновление кадра
    BeginDrawing();
    ClearBackground(RAYWHITE);
    if(!inGame) {
      menu.draw();
    } else {
      game.draw();
    }
    EndDrawing();

    // Обработка выбора пункта меню
    if(selectedOption != -1) {
      if (selectedOption == 0) {
        game = Game("nonograms/Snowflake.txt", screenWidth, screenHeight);
        inGame = true;
        game.runGame();

      } else if (selectedOption == 1) {
        std::cout << "Load level" << std::endl;
      } else if (selectedOption == 2) {
        std::cout << "Exiting!" << std::endl;
        break;
      }
      selectedOption = -1;
    }
    if(inGame && WindowShouldClose()) {
      inGame = false;
    }
  }

  // Закрытие окна
  CloseWindow();
  return 0;
}
