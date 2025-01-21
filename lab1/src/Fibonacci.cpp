#include "./include/Fibonacci.hpp"
#include "../exception/CacheException.hpp"
#include "../exception/FibonacciE.hpp"

int Fibonacci::fib(const int& key) {
    if (key < 0) throw NumFibLessZero("The number of the fibonacci number must be at least 0.");
    
    if (key <= 1) return key;

    int a = 0, b = 1, temp;
    for (int i = 2; i <= key; ++i) {
        temp = a + b;
        a = b;
        b = temp;
    }
    return b;
}

int Fibonacci::calculate(const int& key) {
    if (key < 0) throw NumFibLessZero("The number of the fibonacci number must be at least 0.");
    
    try {
        return cache->get(key);

    } catch(const KeyNotFoundException& e) {
        int result = fib(key);
        cache->put(key, result);

        return result;
    }
}