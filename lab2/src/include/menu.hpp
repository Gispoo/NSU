#ifndef MENU_H
#define MENU_H

#include <vector>
#include <string>
#include "raylib.h"

class Menu {
public:
    Menu(const std::vector<std::string>& options, int screenWidth, int screenHeight);

    void draw() const;

    // ����� ��� ��������� ����� ������������
    int handleInput();

private:
    std::vector<std::string> options; // ������ ����
    int selectedOption;              // ������ ���������� ������
    int screenWidth;    
    int screenHeight;
    int fontSize = 30;
};

#endif

