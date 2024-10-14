#include "./include/CountWords.hpp"

void split_string(std::map <std::string, int>& dictionary, std::string line, char separator, float* num) {
    std::stringstream ss (line);
    std::string word;

    while(getline(ss, word, separator)) {
        ++dictionary[word];
        ++(*num);
    }
}

void CountWords::count_words(std::ifstream& in) {
    while(getline(in, line)) {
        std::stringstream ss(line);
        std::string word;

        split_string(dictionary, line, ' ', &num);
    }
    std::cout << "The words have been successfully counted\n\n";
}