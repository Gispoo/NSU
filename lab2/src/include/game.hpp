#ifndef GAME_H
#define GAME_H

#include "./nonogram.hpp"
#include "./grid.hpp"
#include "./config.h"
#include <string>
#include <filesystem>

class Game {
public:
    Game() : is_game_running(false) {}
    Game(std::filesystem::path filePath);

    void run_game();
    void draw() const;
    void handle_input();
    void check_solution();
    
    bool is_game_running;
    bool is_win = false;
private:
    int screen_width = SCREEN_WIDTH;
    int screen_height = SCREEN_HEIGHT;
    Nonogram current_level;
    int rows;
    int cols;
    Nonogram user_puzzle;
    Grid grid;
};

#endif

