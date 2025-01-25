#ifndef FIBONACCI_H
#define FIBONACCI_H

#include <memory>
#include "./ICache.hpp"
#include "./LFUCache.hpp"
#include "./LRUCache.hpp"

class Fibonacci {
public:
    Fibonacci() = default;
    void setCache(std::unique_ptr<ICache<int, int>>&& cache) { 
        this->cache = std::move(cache); 
    }
    int calculate(const int& key);
    void print_cache() { cache->print_cache(); }

    int fibNumber;

private:
    int fib(const int& key);

    std::unique_ptr<ICache<int, int>> cache;
};

#endif