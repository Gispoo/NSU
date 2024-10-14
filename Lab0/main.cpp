// Copyright 2024 Dima
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include "./src/include/CountWords.hpp"
#include "./src/include/Print.hpp"
#include "./src/include/Start.hpp"

int main(int argc, char* argv[]) {
    try {
        checking_input(argc);

        Start start(argv[1], argv[2]);
        start.open_in_file();

        CountWords cout_words;
        cout_words.count_words(start.in);
        start.in.close();

        Print print;
        print.sort_dictionary(cout_words.dictionary);

        start.open_out_file();
        print.print_dictionary(cout_words.dictionary, cout_words.num, start.out);

        std::cout << "All files were successfully closed. "
        << "The program has been completed\n\n";
    } catch(const std::string& error_masseg) {
        std::cout << error_masseg;
    }

    return 0;
}
