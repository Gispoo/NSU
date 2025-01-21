#include "./include/menu.hpp"
#include "raylib.h"

Menu::Menu(const std::vector<std::string>& options, int screenWidth, int screenHeight) : 
    options(options), selectedOption(0), screenWidth(screenWidth), screenHeight(screenHeight) {}

void Menu::draw() const {
    int textHeight = MeasureText("A", fontSize);
    for (int i = 0; i < options.size(); ++i) {
        int yPos = (screenHeight / 2) - (textHeight * options.size() / 2) + ((textHeight + 15) * i);
        int xPos = (screenWidth - MeasureText(options[i].c_str(), fontSize)) / 2;

        if (i == selectedOption) {
            DrawText(options[i].c_str(), xPos, yPos, fontSize, RED);
        } else {
            DrawText(options[i].c_str(), xPos, yPos, fontSize, BLACK);
        }
    }
}

int Menu::handleInput() {   
    if (IsKeyPressed(KEY_DOWN)) {
        selectedOption = (selectedOption + 1) % options.size();
    } else if (IsKeyPressed(KEY_UP)) {
        selectedOption = (selectedOption - 1 + options.size()) % options.size();
    } else if (IsKeyPressed(KEY_ENTER)) {
      return selectedOption;
    }
    return -1;
}
