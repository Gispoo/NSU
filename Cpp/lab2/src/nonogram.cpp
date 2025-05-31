#include "./include/nonogram.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

Nonogram::Nonogram(std::filesystem::path filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Couldn't open the file:" + filePath.string());
    }

    std::string line;
    std::getline(file, line);
    std::stringstream ss(line);
    ss >> rows >> cols;

    solution.resize(rows, std::vector<int>(cols));
    for (int i = 0; i < rows; ++i) {
        std::getline(file, line);
        for (int j = 0; j < cols; ++j) {
            solution[i][j] = line[j] - '0';
        }
    }
    file.close();

    row_clues.resize(rows);
    for(int i = 0; i < rows; ++i) {
        int sum = 0;
        for (int j = 0; j < cols; ++j) {
            sum += solution[i][j];
            if(sum > 0 && (solution[i][j] == 0 || j == cols - 1)) {
                row_clues[i].push_back(sum);
                sum = 0;
            }
        }
    }

    col_clues.resize(cols);
    for(int i = 0; i < cols; ++i) {
        int sum = 0;
        for (int j = 0; j < rows; ++j) {
            sum += solution[j][i];
            if(sum > 0 && (solution[j][i] == 0 || j == rows - 1)) {
                col_clues[i].push_back(sum);
                sum = 0;
            }
        }
    }
}

Nonogram::Nonogram(int rows, int cols) : rows(rows), cols(cols) {
    solution.resize(rows, std::vector<int>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            solution[i][j] = 0;
        }
    }
}

bool Nonogram::is_solved(Nonogram nonogram) const {
    if (nonogram.rows != rows || nonogram.cols != cols) {
        return false;
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if ((solution[i][j] == 1 && nonogram.solution[i][j] != solution[i][j]) 
            || (solution[i][j] == 0 && nonogram.solution[i][j] == 1)) {
                return false;
            }
        }
    }
    return true;
}

