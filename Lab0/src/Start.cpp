#include <iostream>
#include <fstream>
#include "./include/Start.hpp"

void checking_input(int argc) {
    if (argc != 3) {
        throw std::string{"Incorrect number of arguments.\nSee the examples of calling the program in ReadMe.md\n"};
    }
}

Start::Start(std::string input_file, std::string output_file) {
    name_input_file = input_file;
    name_out_file = output_file;
}

void Start::open_in_file() {
    in.open(name_input_file);

    if (!in.is_open()) {
        throw std::string{"Error opening input file.\nSee the examples of calling the program in ReadMe.md\n"};
    }
    
    std::cout << "The input file has opened successfully\n";
}

void Start::open_out_file() {
    out.open(name_out_file);

    if (!out.is_open()) {
        throw std::string{"Error opening the output file\n.See the examples of calling the program in ReadMe.md\n"};
    }
    
    std::cout << "The output file has opened successfully\n";
}

Start::~Start() {
    in.close();
    out.close();
}