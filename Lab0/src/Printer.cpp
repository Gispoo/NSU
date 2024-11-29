#include "./include/Printer.hpp"
#include <algorithm>
#include <map>
#include <vector>

Printer::Printer(std::string output_file) {
    name_output_file = output_file;
}

void Printer::open_output_file() {
    output_file.open(name_output_file);

    if (!output_file.is_open()) {
        throw std::runtime_error("Error opening " + name_output_file + " file\n.See the examples of calling the program in ReadMe.md\n");
    }
    
    std::cout << "The output file has opened successfully\n";
}


void Printer::print_dictionary(std::map <std::string, int> &dictionary, std::list <std::string> sorted_dictionary, float num) {
    for (std::string word : sorted_dictionary) {
       output_file << word << ", " << dictionary[word] << ", " << dictionary[word] / num * 100.0 << "%\n";
    }

    std::cout << "The program has successfully printed a table of word frequencies\n\n";
}

Printer::~Printer() {
    output_file.close();
}
