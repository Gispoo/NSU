#ifndef RUNPROGRAM_H
#define RUNPROGRAM_H

#include "./menu.hpp"
#include "./game.hpp"
#include "./Levels.hpp"
#include <string>
#include <filesystem>

class RunProgram {
public:
    void run();

private:
    Levels levels;
    std::filesystem::path level_path;

    void get_levels_paths();
    void start_menu_item(int menu_item);
};

#endif
