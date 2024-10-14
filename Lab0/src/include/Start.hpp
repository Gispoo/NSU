#ifndef START_H
#define START_H

#include <fstream>

void checking_input(int argc);

class Start {
private:
    std::string name_input_file, name_out_file;
    
public:
    std::ifstream in;
    std::ofstream out;

    Start(std::string input_file, std::string output_file);    

    void open_in_file();

    void open_out_file();

    ~Start();
};

#endif