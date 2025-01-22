#ifndef NONOGRAM_H
#define NONOGRAM_H

#include <vector>
#include <string>
#include <filesystem>

class Nonogram {
public:
    Nonogram(std::filesystem::path filePath);
    Nonogram(int rows, int cols);
    Nonogram() = default;

    bool is_solved(Nonogram nonogram) const;

    int rows;
    int cols;
    std::vector<std::vector<int>> row_clues;
    std::vector<std::vector<int>> col_clues;
    std::vector<std::vector<int>> solution;
};

#endif

