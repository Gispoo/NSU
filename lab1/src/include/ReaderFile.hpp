#ifndef READERFILE_H
#define READERFILE_H

#include "./Reader.hpp"

class ReaderFile : public Reader {
public:
    ReaderFile(int num_fib, char* argv[]) : Reader(num_fib, argv) {}

    int get_num_fib_num() override;

    class CheckerFile : public Checker {
    public:
        bool check_arg() override;
    };
private:
};

#endif