#ifndef COUNTWORDS_H
#define COUNTWORDS_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>

void split_string(std::map <std::string, int> &dictionary, std::string &line, char separator, float* num);

class CounterWord {
private:
    std::string line;

public:
    std::map <std::string, int> dictionary;
    float num = 0;
    
    void count_word(std::ifstream& input_file);
};

#endif