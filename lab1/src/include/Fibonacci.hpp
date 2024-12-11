#ifndef FIBONACCI_H
#define FIBONACCI_H

#include "./ICacheable.hpp"
#include "./ReaderFile.hpp"

class Fibonacci {
private:
    ICacheable& interface;

public:
    Fibonacci(ICacheable& I) : interface(I) {}

    int get_fib_num(int n);
};

#endif