#ifndef READER_H
#define READER_H

#include <fstream>

class Reader {
private:
    std::string name_input_file;
    
public:
    std::ifstream input_file;

    Reader(std::string input_file);    

    void open_input_file();

    ~Reader();
};

#endif