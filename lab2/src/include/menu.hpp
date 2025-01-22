#ifndef MENU_H
#define MENU_H

#include "./config.h"
#include <vector>
#include <string>

enum Options {
    INVALID = -1,
    LEVEL1 = 0,
    LEVEL2 = 1,
    LEVEL3 = 2,
    LEVEL4 = 3,
    LEVEL5 = 4,
    EXIT = 5
};

class Menu {
public:
    Menu(const std::vector<std::string>& options) : options(options) {}

    void draw() const;
    Options handle_input();

private:
    Options selected_option = LEVEL1;
    std::vector<std::string> options;
    int screen_width = SCREEN_WIDTH;
    int screen_height = SCREEN_HEIGHT;
    int font_size = FONT_SIZE;
};

#endif

