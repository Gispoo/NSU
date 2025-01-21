#ifndef SILENCECONVERTER_H
#define SILENCECONVERTER_H

#include "Converter.hpp"
#include <vector>
#include <cstdint>

class SilenceConverter : public Converter {
public:
    SilenceConverter();
    ~SilenceConverter() override = default;
    std::vector<int16_t> process(const std::vector<int16_t>& input) override;
    void setParameters(const std::vector<std::string>& params) override;
private:
    double startSec = 0;
    double endSec = 0;

};

#endif
