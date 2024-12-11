// Copyright 2024 Dima
#include <string>
#include <iostream>
#include "./src/include/LFUCache.hpp"
#include "./src/include/LRUCache.hpp"
#include "./src/include/ICacheable.hpp"
#include "./src/include/Fibonacci.hpp"
#include "./src/include/Reader.hpp"
#include "./src/include/ReaderFile.hpp"

int main(int argc, char* argv[]) {
    try {
        ReaderFile ReaderFile(argc, argv);
        Reader& Reader = ReaderFile;

        int n = Reader.get_num_fib_num();

        LRUCache Rcashe;
        LFUCache Fcashe;

        ICacheable& InterfaceR = Rcashe;
        ICacheable& InterfaceF = Fcashe;

        Fibonacci FibR(InterfaceR);
        Fibonacci FibF(InterfaceF);

        FibR.get_fib_num(n);
        FibF.get_fib_num(n);

    } catch(const std::string& error_masseg) {
        std::cout << error_masseg;
    }

    return 0;
}
