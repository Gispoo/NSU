#include "./include/menu.hpp"
#include "./include/RunProgram.hpp"
#include "raylib.h"

void Menu::draw() const {
    int textHeight = MeasureText("A", font_size);
    for (int i = 0; i < options.size(); ++i) {
        int yPos = (screen_height / 2) - (textHeight * options.size() / 2) + ((textHeight + 15) * i);
        int xPos = (screen_width - MeasureText(options[i].c_str(), font_size)) / 2;

        if (i == selected_option) {
            DrawText(options[i].c_str(), xPos, yPos, font_size, RED);
        } else {
            DrawText(options[i].c_str(), xPos, yPos, font_size, BLACK);
        }
    }
}

Options Menu::handle_input() {
    if (IsKeyPressed(KEY_DOWN)) {
        int nextOption = static_cast<int>(selected_option) + 1;
        if (nextOption > static_cast<int>(Options::EXIT)) {
            selected_option = LEVEL1;
        } else {
            selected_option = static_cast<Options>(nextOption);
        }
    } else if (IsKeyPressed(KEY_UP)) {
        int prevOption = static_cast<int>(selected_option) - 1;
        if (prevOption < static_cast<int>(Options::LEVEL1)) {
            selected_option = EXIT;
        } else {
        selected_option = static_cast<Options>(prevOption);
        }
    } else if (IsKeyPressed(KEY_ENTER)) {
        return selected_option;
    }
    return INVALID;
}
