#include "./include/nonogram.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Nonogram::Nonogram(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Couldn't open the file:" + filePath);
    }

    std::string line;
    //—читывание размера
    std::getline(file, line);
    std::stringstream ss(line);
    ss >> rows >> cols;

    //—читывание решени€
    solution.resize(rows, std::vector<int>(cols));
    for (int i = 0; i < rows; ++i) {
        std::getline(file, line);
        for (int j = 0; j < cols; ++j) {
            solution[i][j] = line[j] - '0';
        }
    }
    file.close();

    //—читывание подсказок дл€ строк
    rowClues.resize(rows);
    for(int i = 0; i < rows; ++i) {
        int sum = 0;
        for (int j = 0; j < cols; ++j) {
            sum += solution[i][j];
            if(sum > 0 && (solution[i][j] == 0 || j == cols - 1)) {
                rowClues[i].push_back(sum);
                sum = 0;
            }
        }
    }

    //—читывание подсказок дл€ столбцов
    colClues.resize(cols);
    for(int i = 0; i < cols; ++i) {
        int sum = 0;
        for (int j = 0; j < rows; ++j) {
            sum += solution[j][i];
            if(sum > 0 && (solution[j][i] == 0 || j == rows - 1)) {
                colClues[i].push_back(sum);
                sum = 0;
            }
        }
    }

}

bool Nonogram::isSolved(const std::vector<std::vector<int>>& userGrid) const {
    if (userGrid.size() != rows || userGrid[0].size() != cols) {
        return false;
    }

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if ((solution[i][j] == 1 && userGrid[i][j] != solution[i][j]) || (solution[i][j] == 0 && userGrid[i][j] == 1)) {
                return false;
            }
        }
    }
    return true;
}

