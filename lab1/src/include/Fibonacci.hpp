#ifndef FIBONACCI_H
#define FIBONACCI_H

#include <memory>
#include "./ICache.hpp"

class Fibonacci {
public:
    int fibNumber;

    Fibonacci(std::unique_ptr<ICache<int, int>> cache) : cache(std::move(cache)) {}
    int calculate(const int& key);

private:
    std::unique_ptr<ICache<int, int>> cache;
    int fibonacci(const int& n);
};

#endif