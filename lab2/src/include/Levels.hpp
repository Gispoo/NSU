#ifndef LEVELS_H
#define LEVELS_H

#include <string>
#include <filesystem>
#include "./nonogram.hpp"

class Levels {
public:
    std::filesystem::path dir_path_level = "/home/gispo/C-/lab2/nonograms";
    static const std::vector<std::string> menuOptions;
    std::vector<std::filesystem::path> levels_paths;

};

#endif
