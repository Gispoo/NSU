#include "./include/CounterWord.hpp"

void split_string(std::map <std::string, int> &dictionary, std::string &line, char separator, float* num) {
    std::stringstream ss(line);
    std::string word;

    while(getline(ss, word, separator)) {
        ++dictionary[word];
        ++(*num);
    }
}

void CounterWord::count_word(std::ifstream& input_file) {
    while(getline(input_file, line)) {
        split_string(dictionary, line, ' ', &num);
    }
    std::cout << "The words have been successfully counted\n\n";
}