#ifndef RUNPROGRAMM_H
#define RUNPROGRAMM_H

#include <string>
#include "./Fibonacci.hpp"
#include "./LFUCache.hpp"
#include "./LRUCache.hpp"

typedef struct MessagList{
    std::string message0 = "\nS - select the cache type, E - enter the Fibonacci number, V - view cache\nSelect an action:\n";
    std::string message00 = "Wrong action\n";
    std::string message1 = "Select the cache type: LRU - R, LFU - F\n";
    std::string message2 = "Invalid cache type:\n";
    std::string message3 = "Enter the cache size:\n";
    std::string message4 = "The size is incorrect:\n";
    std::string message5 = "Enter the number of the Fibonacci number:\n";
    std::string message6 = "The number is incorrect:\n";
    std::string message7 = "The Fibonacci number under the number ";
} MessagList;

class RunProgramm {
public:
    void run();

private:
    char get_action();
    void get_size_cache();
    void get_type_cache();
    int get_num();

    MessagList messages;
    Fibonacci Fib;
    int size_cache; 
};

#endif