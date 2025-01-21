#ifndef RUNPROGRAM_H
#define RUNPROGRAM_H

#include <string>
#include "./Fibonacci.hpp"
#include "./LFUCache.hpp"
#include "./LRUCache.hpp"

enum class Action {
    SELECT,
    ENTER,
    VIEW,
    INVALID
};

typedef struct MessagList{
    std::string menu_action = "\nS - select the cache type, E - enter the Fibonacci number, V - view cache\nSelect an action:\n";
    std::string wrong_action = "Wrong action\n";
    std::string cache_selection = "Select the cache type: LRU - R, LFU - F\n";
    std::string invalid_cache_type = "Invalid cache type:\n";
    std::string enter_cache_size = "Enter the cache size:\n";
    std::string incorret_size = "The size is incorrect:\n";
    std::string new_num_fib = "Enter the number of the Fibonacci number:\n";
    std::string incorrect_num = "The number is incorrect:\n";
    std::string output_num_fib = "The Fibonacci number under the number ";
} MessagList;

class RunProgram {
public:
    void run();

private:
    Action get_action();
    void get_size_cache();
    void get_type_cache();
    int get_num();

    MessagList messages;
    Fibonacci Fib;
    int size_cache; 
};

#endif