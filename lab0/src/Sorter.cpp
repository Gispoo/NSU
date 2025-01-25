#include "./include/Sorter.hpp"
#include <algorithm>
#include <map>
#include <vector>
#include <utility>

void Sorter::sort_dictionary(std::map<std::string, int>& dictionary) {
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
