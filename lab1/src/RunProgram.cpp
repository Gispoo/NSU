#include <iostream>
#include <memory>
#include <limits>
#include <string>
#include "./include/RunProgram.hpp"
#include "./include/LRUCache.hpp"
#include "./include/LFUCache.hpp"
#include "./include/Fibonacci.hpp"

const std::string MessageConsts::MENU_ACTION = "\nS - select the cache type, E - enter the Fibonacci number, V - view cache\nSelect an action:\n";
const std::string MessageConsts::WRONG_ACTION = "Wrong action\n";
const std::string MessageConsts::CACHE_SELECTION = "Select the cache type: LRU - R, LFU - F\n";
const std::string MessageConsts::INVALID_CACHE_TYPE = "Invalid cache type:\n";
const std::string MessageConsts::ENTER_CACHE_SIZE = "Enter the cache size:\n";
const std::string MessageConsts::INCORRECT_SIZE = "The size is incorrect:\n";
const std::string MessageConsts::NEW_NUM_FIB = "Enter the number of the Fibonacci number:\n";
const std::string MessageConsts::INCORRECT_NUM = "The number is incorrect:\n";
const std::string MessageConsts::OUTPUT_NUM_FIB = "The Fibonacci number under the number ";


Action RunProgram::get_action() {
    while (true) {
        char user_input;
        std::cout << messages.MENU_ACTION;
        std::cin >> user_input;

        if (std::cin.fail()) {
            std::cout << messages.WRONG_ACTION;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        Action action = Action::INVALID;
        if(user_input == 'S'){
           action = Action::SELECT;
        } else if(user_input == 'E'){
           action = Action::ENTER;
        } else if (user_input == 'V'){
           action = Action::VIEW;
        }

        if(action == Action::INVALID){
          std::cout << messages.WRONG_ACTION;
          continue;
        }

        return action;
    }
}

void RunProgram::get_size_cache() {
    while(true) {
        std::cout << messages.ENTER_CACHE_SIZE;
        std::cin >> size_cache;

        if (std::cin.fail() || size_cache < 1) {
            std::cout << messages.INCORRECT_SIZE;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        break;
    }
}

void RunProgram::get_type_cache() {
    while (true) {
        char type_cache;
        std::cout << messages.CACHE_SELECTION;
        std::cin >> type_cache;

        if (std::cin.fail() || (type_cache != 'R' && type_cache != 'F')) {
            std::cout << messages.INVALID_CACHE_TYPE;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        if (type_cache == 'R') {
            std::unique_ptr<LRUCache<int, int>> p_Rcache = std::make_unique<LRUCache<int, int>>(size_cache);
            Fib.setCache(std::move(p_Rcache));
        } else {
            std::unique_ptr<LFUCache<int, int>> p_Fcache = std::make_unique<LFUCache<int, int>>(size_cache);
            Fib.setCache(std::move(p_Fcache));
        }

        break;
    }
}

int RunProgram::get_num() {
    while(true) {
        int num;
        std::cout << messages.NEW_NUM_FIB;
        std::cin >> num;

        if (std::cin.fail() || num < 0) {
            std::cout << messages.INCORRECT_NUM;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        return num;
    }
}

void RunProgram::run() {
    try {
        Action action = Action::SELECT;
        while(true) {
            switch(action) {
            case Action::SELECT:
                get_size_cache();
                get_type_cache();
                Fib.fibNumber = get_num();
                std::cout << messages.OUTPUT_NUM_FIB << Fib.calculate(Fib.fibNumber) << std::endl;
                break;
            case Action::ENTER:
                Fib.fibNumber = get_num();
                std::cout << messages.OUTPUT_NUM_FIB << Fib.calculate(Fib.fibNumber) << std::endl;
                break;
            case Action::VIEW:
                Fib.print_cache();
                break;
            }

            action = get_action();
        }

    } catch(SmallSizeCacheException& e) {
        std::cerr << e.what() << std::endl;
        return;
    }
}