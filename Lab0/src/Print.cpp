#include "./include/print.hpp"

void Print::sort_dictionary(std::map <std::string, int> dictionary) {
    int size_map = dictionary.size();
    for (int i = 0; i < size_map; ++i) {
        int max = -1;
        std::string max_word;
        for (const auto& [key, value] : dictionary) {
            if (value > max) {
                max = value;
                max_word = key;
            }
        }
        sorted_dictionary.push_back(max_word);
        dictionary.erase(max_word);
    }
}

void Print::print_dictionary(std::map <std::string, int> dictionary, float num, std::ofstream& out) {
    for (std::string word : sorted_dictionary) {
       out << word << ", " << dictionary[word] << ", " << dictionary[word] / num * 100.0 << "%\n";
    }

    std::cout << "The program has successfully printed a table of word frequencies\n\n";
}
