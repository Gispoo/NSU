#ifndef PRINTER_H
#define PRINERT_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <list>

class Printer {
public:
    std::ofstream output_file;

    Printer(std::string output_file);

    void open_output_file();
    
    void print_dictionary(std::map <std::string, int> &dictionary, std::list <std::string> sorted_dictionary, float num);

    ~Printer();
private:
    std::string name_output_file;
};

#endif