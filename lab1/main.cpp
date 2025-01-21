// Copyright 2024 Dima
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include "./src/include/LFUCache.hpp"
#include "./src/include/LRUCache.hpp"
#include "./src/include/ICache.hpp"
#include "./src/include/Fibonacci.hpp"
#include "./src/include/RunProgram.hpp"
#include "./exception/CacheException.hpp"

int main(int argc, char* argv[]) {
    try {
        RunProgram programm;
        programm.run();
        
    } catch(SmallSizeCacheException& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }
}
