#ifndef READER_H
#define READER_H

#include <fstream>

class ArgumentReader {
private:
    std::string name_input_file;
    
public:
    std::ifstream input_file;

    ArgumentReader(std::string input_file);    

    void open_input_file();

    ~ArgumentReader();
};

#endif