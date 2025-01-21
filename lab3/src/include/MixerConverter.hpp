#ifndef MIXERCONVERTER_H
#define MIXERCONVERTER_H

#include "Converter.hpp"
#include <vector>
#include <cstdint>

class MixerConverter : public Converter {
public:
    MixerConverter();
    ~MixerConverter() override = default;
    std::vector<int16_t> process(const std::vector<int16_t>& input) override;
    void setParameters(const std::vector<std::string>& params) override;
    void setMixFile(const std::string& mixFilename);
private:
    double insertTimeSec = 0;
    std::vector<int16_t> mixSamples;
    bool hasMix = false;
};

#endif
