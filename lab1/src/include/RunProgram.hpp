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

class MessageConsts {
public:
    static const std::string MENU_ACTION;
    static const std::string WRONG_ACTION;
    static const std::string CACHE_SELECTION;
    static const std::string INVALID_CACHE_TYPE;
    static const std::string ENTER_CACHE_SIZE;
    static const std::string INCORRECT_SIZE;
    static const std::string NEW_NUM_FIB;
    static const std::string INCORRECT_NUM;
    static const std::string OUTPUT_NUM_FIB;
};

class RunProgram {
public:
    void run();

private:
    Action get_action();
    void get_size_cache();
    void get_type_cache();
    int get_num();

    MessageConsts messages;
    Fibonacci Fib;
    int size_cache; 
};

#endif