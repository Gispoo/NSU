#include <iostream>
#include <memory>
#include <limits>
#include <string>
#include "./include/RunProgram.hpp"
#include "./include/LRUCache.hpp"
#include "./include/LFUCache.hpp"
#include "./include/Fibonacci.hpp"

Action RunProgram::get_action() {
    while (true) {
        char user_input;
        std::cout << messages.menu_action;
        std::cin >> user_input;

        if (std::cin.fail()) {
            std::cout << messages.wrong_action;
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
          std::cout << messages.wrong_action;
          continue;
        }

        return action;
    }
}

void RunProgram::get_size_cache() {
    while(true) {
        std::cout << messages.enter_cache_size;
        std::cin >> size_cache;

        if (std::cin.fail() || size_cache < 1) {
            std::cout << messages.incorret_size;
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
        std::cout << messages.cache_selection;
        std::cin >> type_cache;

        if (std::cin.fail() || (type_cache != 'R' && type_cache != 'F')) {
            std::cout << messages.invalid_cache_type;
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
        std::cout << messages.new_num_fib;
        std::cin >> num;

        if (std::cin.fail() || num < 0) {
            std::cout << messages.incorrect_num;
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
                std::cout << messages.output_num_fib << Fib.calculate(Fib.fibNumber) << std::endl;
                break;
            case Action::ENTER:
                Fib.fibNumber = get_num();
                std::cout << messages.output_num_fib << Fib.calculate(Fib.fibNumber) << std::endl;
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