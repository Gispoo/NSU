#ifndef NONOGRAM_H
#define NONOGRAM_H

#include <vector>
#include <string>

class Nonogram {
public:
    // Конструктор, загружающий кроссворд из файла
    Nonogram(const std::string& filePath);

    // Проверяет решен ли кроссворд
    bool isSolved(const std::vector<std::vector<int>>& userGrid) const;

    int rows;                      // Количество строк
    int cols;                      // Количество столбцов
    std::vector<std::vector<int>> rowClues; // Подсказки для строк
    std::vector<std::vector<int>> colClues; // Подсказки для столбцов
    std::vector<std::vector<int>> solution; // Правильное решение кроссворда
};

#endif

