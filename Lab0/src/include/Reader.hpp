#ifndef START_H
#define START_H

#include <fstream>

void checking_input(int argc);

class Reader {
private:
    std::string name_input_file, name_output_file;
    
public:
    std::ifstream input_file;
    std::ofstream output_file;

    Reader(std::string input_file, std::string output_file);    

    void open_input_file();

    void open_output_file();

    ~Reader();
};

#endif