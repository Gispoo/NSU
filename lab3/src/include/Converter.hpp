#ifndef CONVERTER_H
#define CONVERTER_H

#include <vector>
#include <cstdint>
#include <string>

class Converter {
public:
    virtual ~Converter() = default;
    virtual std::vector<int16_t> process(const std::vector<int16_t>& input) = 0;
    virtual void setParameters(const std::vector<std::string>& params) = 0;

};

#endif
