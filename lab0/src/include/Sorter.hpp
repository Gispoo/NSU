#ifndef SORTER_H
#define SORTER_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <list>

class Sorter {
public:
    void sort_dictionary(std::map <std::string, int> &dictionary);

    std::list <std::string> sorted_dictionary;
};

#endif