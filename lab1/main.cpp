// Copyright 2024 Dima
#include <string>
#include <iostream>
#include <memory>
#include "./src/include/LFUCache.hpp"
#include "./src/include/LRUCache.hpp"
#include "./src/include/ICache.hpp"
#include "./src/include/Fibonacci.hpp"
#include "./src/include/ArgumentReader.hpp"

int main(int argc, char* argv[]) {
    ArgumentReader argument_reader;
    try {
        auto cache = std::make_unique<LRUCache<int, int>>(10);
        Fibonacci fib(std::move(cache));

        fib.fibNumber = argument_reader.parse(argc, argv);
        long long result = fib.calculate(fib.fibNumber);
        std::cout << "Fibonacci(" << fib.fibNumber << ") = " << result << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
