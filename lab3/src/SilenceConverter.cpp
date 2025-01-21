#include "./include/SilenceConverter.hpp"
#include <iostream>
#include <stdexcept>
#include <cstdlib>

SilenceConverter::SilenceConverter() {}

void SilenceConverter::setParameters(const std::vector<std::string>& params) {
    if (params.size() != 2) {
        throw std::runtime_error("SilenceConverter: Requires two parameters: start_time and end_time.");
    }

    try {
        startSec = std::stod(params[0]);
        endSec = std::stod(params[1]);
    }
    catch (const std::invalid_argument& e) {
        throw std::runtime_error("SilenceConverter: Invalid format of arguments. Start time and end time must be numbers");
    }

    if (startSec < 0 || endSec < 0 || startSec > endSec) {
        throw std::runtime_error("SilenceConverter: Invalid values of arguments. Start and end time must be positive numbers where start time < end time");
    }
}

std::vector<int16_t> SilenceConverter::process(const std::vector<int16_t>& input) {
    if (startSec < 0 || endSec < 0 ) {
          return input;
    }
    
    int sampleRate = 44100;
    int startSample = static_cast<int>(startSec * sampleRate);
    int endSample = static_cast<int>(endSec * sampleRate);

    std::vector<int16_t> output = input;
    for (int i = startSample; i < endSample && i < output.size(); ++i) {
        output[i] = 0;
    }
    return output;
}
