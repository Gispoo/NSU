#ifndef CUSTOMCONVERTER_H
#define CUSTOMCONVERTER_H

#include "Converter.hpp"
#include <vector>
#include <cstdint>

class CustomConverter : public Converter {
public:
    CustomConverter();
    ~CustomConverter() override = default;
    std::vector<int16_t> process(const std::vector<int16_t>& input) override;
    void setParameters(const std::vector<std::string>& params) override;
private:
    double gainFactor = 1.0;
};

#endif
