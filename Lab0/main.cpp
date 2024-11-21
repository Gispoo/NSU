// Copyright 2024 Dima
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <stdexcept>
#include "./src/include/CounterWord.hpp"
#include "./src/include/Printer.hpp"
#include "./src/include/Reader.hpp"

void checking_input(int argc) {
    if (argc != 3) {
        throw std::runtime_error("Incorrect number of arguments.\nSee the examples of calling the program in ReadMe.md\n");
    }
}

int main(int argc, char* argv[]) {
    try {
        checking_input(argc);

        Reader reader(argv[1], argv[2]);
        reader.open_input_file();

        CounterWord couter_word;
        couter_word.count_word(reader.input_file);
        reader.input_file.close();

        Printer printer;
        printer.sort_dictionary(couter_word.dictionary);

        reader.open_output_file();
        printer.print_dictionary(couter_word.dictionary, couter_word.num, reader.output_file);

        std::cout << "All files were successfully closed. "
        << "The program has been completed\n\n";
    } catch(const std::string& error_masseg) {
        std::cout << error_masseg;
    }

    return 0;
}
