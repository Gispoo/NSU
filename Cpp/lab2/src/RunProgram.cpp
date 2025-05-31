#include "raylib.h"
#include "./include/RunProgram.hpp"
#include "./include/menu.hpp"
#include "./include/game.hpp"
#include "./include/Levels.hpp"
#include "./include/config.h"
#include <vector>
#include <string>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

const std::vector<std::string> Levels::menuOptions = {
    "Level 1",
    "Level 2",
    "Level 3",
    "Level 4",
    "Level 5",
    "Exit"
};

void RunProgram::run() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Nonogram Game");
    SetTargetFPS(FPS);

    get_levels_paths();
    level_path = levels.levels_paths[0];
    
    Menu menu(levels.menuOptions);
    Game game;
    while (!WindowShouldClose()) {
        if (!game.is_game_running) {
            int menu_item = static_cast<int>(menu.handle_input());
            if (menu_item != -1) {
                start_menu_item(menu_item);
                game = Game(level_path);
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (!game.is_game_running) {
            menu.draw();
        } else {
            game.run_game();
            game.draw();
        }
        EndDrawing();

        if (game.is_game_running && WindowShouldClose()) {
            game.is_game_running = false;
        }
    }

    CloseWindow();
}

void RunProgram::get_levels_paths() {
    if (fs::exists(levels.dir_path_level) && fs::is_directory(levels.dir_path_level)) {
        for (const auto& entry : fs::directory_iterator(levels.dir_path_level)) {
            if (fs::is_regular_file(entry.path())) {
                levels.levels_paths.push_back(entry.path());
            }
        }
    } else {
        std::cerr << "Error: directory '" << levels.dir_path_level << "' does not exist or is not a directory.\n";
        return;
    }
}

void RunProgram::start_menu_item(int menu_item) {
    switch (menu_item) {
    case 0:
        level_path = levels.levels_paths[0];
        break;
    case 1:
        level_path = levels.levels_paths[1];
        break;
    case 2:
        level_path = levels.levels_paths[2];
        break;
    case 3:
        level_path = levels.levels_paths[3];
        break;
    case 4:
        level_path = levels.levels_paths[3];
        break;
    case 5:
        CloseWindow();
    }
}