// Copyright 2024 Dima
#include <string>
#include <iostream>
#include "./src/include/LFUCache.hpp"
#include "./src/include/LRUCache.hpp"
#include "./src/include/ICacheable.hpp"
#include "./src/include/Fibonacci.hpp"
#include "./src/include/Reader.hpp"

int main(int argc, char* argv[]) {
    Reader reader;
    try {
        long long fibNumber = reader.readInput(argc, argv);

        auto cache = new LRUCache<long long, long long>(10);
        // Fibonacci<long long, long long> wrapper(cache);

        // long long result = wrapper.calculate(fibNumber);
        // std::cout << "Fibonacci(" << fibNumber << ") = " << result << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
