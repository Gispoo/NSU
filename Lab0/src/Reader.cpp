#include <iostream>
#include <fstream>
#include <stdexcept>
#include "./include/Reader.hpp" 

Reader::Reader(std::string input_file) {
    name_input_file = input_file;
}

void Reader::open_input_file() {
    input_file.open(name_input_file);

    if (!input_file.is_open()) {
        throw std::runtime_error("Error opening " + name_input_file + " file.\nSee the examples of calling the program in ReadMe.md\n");
    }
    
    std::cout << "The input file has opened successfully\n";
}

Reader::~Reader() {
    input_file.close();
}