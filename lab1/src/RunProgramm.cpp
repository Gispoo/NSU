#include <iostream>
#include <memory>
#include <limits>
#include "./include/RunProgramm.hpp"
#include "./include/LRUCache.hpp"
#include "./include/LFUCache.hpp"
#include "./include/Fibonacci.hpp"

char RunProgramm::get_action() {
    while (true) {
        char action;
        std::cout << messages.message0;
        std::cin >> action;

        if (std::cin.fail() || (action != 'E' && action != 'V' && action != 'S')) {
            std::cout << messages.message00;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        return action;
    }
}

void RunProgramm::get_size_cache() {
    while(true) {
        std::cout << messages.message3;
        std::cin >> size_cache;

        if (std::cin.fail() || size_cache < 1) {
            std::cout << messages.message4;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        break;
    }
}

void RunProgramm::get_type_cache() {
    while (true) {
        char type_cache;
        std::cout << messages.message1;
        std::cin >> type_cache;

        if (std::cin.fail() || (type_cache != 'R' && type_cache != 'F')) {
            std::cout << messages.message2;
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

int RunProgramm::get_num() {
    while(true) {
        int num;
        std::cout << messages.message5;
        std::cin >> num;

        if (std::cin.fail() || num < 0) {
            std::cout << messages.message6;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        return num;
    }
}

void RunProgramm::run() {
    try {
        char action = 'S';
        while(true) {
            switch(action) {
            case 'S':
                get_size_cache();
                get_type_cache();
                Fib.fibNumber = get_num();
                std::cout << messages.message7 << Fib.calculate(Fib.fibNumber) << std::endl;
                break;
            case 'E':
                Fib.fibNumber = get_num();
                std::cout << messages.message7 << Fib.calculate(Fib.fibNumber) << std::endl;
                break;
            case 'V':
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