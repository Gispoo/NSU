#ifndef NONOGRAM_H
#define NONOGRAM_H

#include <vector>
#include <string>

class Nonogram {
public:
    // �����������, ����������� ��������� �� �����
    Nonogram(const std::string& filePath);

    // ��������� ����� �� ���������
    bool isSolved(const std::vector<std::vector<int>>& userGrid) const;

    int rows;                      // ���������� �����
    int cols;                      // ���������� ��������
    std::vector<std::vector<int>> rowClues; // ��������� ��� �����
    std::vector<std::vector<int>> colClues; // ��������� ��� ��������
    std::vector<std::vector<int>> solution; // ���������� ������� ����������
};

#endif

