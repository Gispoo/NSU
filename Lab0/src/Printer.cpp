#include "./include/Printer.hpp"
#include <algorithm>
#include <map>
#include <vector>
#include <utility>

void Printer::sort_dictionary(std::map<std::string, int>& dictionary) {
    std::vector<std::pair<int, std::string>> vec;
    for (const auto& pair : dictionary) {
        vec.emplace_back(pair.second, pair.first);
    }

    std::sort(vec.begin(), vec.end(), std::greater<std::pair<int, std::string>>());

    sorted_dictionary.clear();
    for (const auto& pair : vec) {
        sorted_dictionary.push_back(pair.second);
    }
}



void Printer::print_dictionary(std::map <std::string, int> &dictionary, float num, std::ofstream& output_file) {
    for (std::string word : sorted_dictionary) {
       output_file << word << ", " << dictionary[word] << ", " << dictionary[word] / num * 100.0 << "%\n";
    }

    std::cout << "The program has successfully printed a table of word frequencies\n\n";
}
