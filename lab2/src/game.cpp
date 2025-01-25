#include "raylib.h"
#include "./include/game.hpp"
#include "./include/config.h"
#include <iostream>
#include <filesystem>

Game::Game(std::filesystem::path filePath) : 
    is_game_running(true),
    current_level(filePath),
    rows(current_level.rows),
    cols(current_level.cols),
    user_puzzle(rows, cols),
    grid(rows, cols) {}

void Game::run_game() {
    while (is_game_running) {
        handle_input();
        check_solution();
        draw();
        
        if (WindowShouldClose()) {
            is_game_running = false;
        }
    }
}

void Game::draw() const {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    grid.draw();
    grid.draw_clue(current_level.row_clues, current_level.col_clues);

    // Отрисовка сообщения о победе
    // if (is_win) {
    //     DrawText("You Win!", );
    // }

    EndDrawing();
}

void Game::handle_input() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 mousePos = GetMousePosition();
        int row, col;
        if (grid.get_cell_from_mouse_pos((int)mousePos.x, (int)mousePos.y, row, col)) {
            grid.set_state_cell(row, col, (grid.get_state_cell(row, col) + 1) % 2);
        }
    } else if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
        Vector2 mousePos = GetMousePosition();
        int row, col; 
        if (grid.get_cell_from_mouse_pos((int)mousePos.x, (int)mousePos.y, row, col)) {
            int currentState = grid.get_state_cell(row, col);
            int newState;

            if (currentState == 0 || currentState == 1) {
                newState = 2;
            } else {
                newState = 0;
            }
            grid.set_state_cell(row, col, newState);
        }
    }
}

void Game::check_solution() {
    if (user_puzzle.is_solved(current_level)) {
        is_win = true;
        is_game_running = false;
    }
}

