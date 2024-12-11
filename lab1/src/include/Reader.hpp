#ifndef READER_H
#define READER_H

#include <vector>

class Reader {
public:
    Reader(int num_fib, char* argv[]);
    
    virtual int get_num_fib_num() = 0;

    class Checker {
    public:
        virtual bool check_arg() = 0;
    };
private:
    int num_fib;
    std::vector<int> argv;
};

#endif