#ifndef PRINT_H
#define PRINT_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <list>

class Print {
public:
    void sort_dictionary(std::map <std::string, int> dictionary);
    void print_dictionary(std::map <std::string, int> dictionary, float num, std::ofstream& out);
private:
    std::list <std::string> sorted_dictionary;
};

#endif