#ifndef FIBONACCI_H
#define FIBONACCI_H

#include "./ICacheable.hpp"

template <typename K, typename V>
class Fibonacci {
public:
    Fibonacci(ICacheable<K,V>& cache) : cache(cache) {}
    V calculate(const K& key);
    ~Fibonacci() { delete cache; }

private:
    ICacheable<K,V>& cache;
    V fibonacci(const K& n);
};

#endif