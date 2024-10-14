#ifndef COUNTWORDS_H
#define COUNTWORDS_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>

void split_string(std::map <std::string, int>& dictionary, std::string line, char separator, float* num);

class CountWords {
private:
    std::string line;

public:
    std::map <std::string, int> dictionary;
    float num = 0;
    
    void count_words(std::ifstream& in);
};

#endif