#include "./include/MixerConverter.hpp"
#include "./include/WavFile.hpp"
#include <iostream>
#include <stdexcept>
#include <numeric>


MixerConverter::MixerConverter() : insertTimeSec(0) {}

void MixerConverter::setParameters(const std::vector<std::string>& params) {
    if (params.size() > 1) {
        throw std::runtime_error("MixerConverter: Too much arguments. Mixer takes no more than one argument: insert time (default 0)");
    }
    if (params.size() == 1) {
        try {
            insertTimeSec = std::stod(params[0]);
        }
        catch (const std::invalid_argument& e) {
           throw std::runtime_error("MixerConverter: Invalid format of argument. insert time must be a number");
        }
        if (insertTimeSec < 0 ) {
             throw std::runtime_error("MixerConverter: Invalid values of argument. insert time must be a positive number");
        }
    }
    hasMix = !mixSamples.empty();
}
void MixerConverter::setMixFile(const std::string& mixFilename) {
    WavFile mixFile(mixFilename);
    if (!mixFile.isValid()) {
         throw std::runtime_error("MixerConverter: Unable to load mix file");
    }
    mixSamples = mixFile.getSamples();
}

std::vector<int16_t> MixerConverter::process(const std::vector<int16_t>& input) {
    std::vector<int16_t> output = input;
    if (!hasMix) {
        return output;
    }

    int sampleRate = 44100;
    int insertSample = static_cast<int>(insertTimeSec * sampleRate);
    for (size_t i = 0; i < mixSamples.size(); ++i) {
        if (i + insertSample < output.size()) {
            output[i + insertSample] = (output[i + insertSample] + mixSamples[i]) / 2;
        }
        else {
            break;
        }
    }

    return output;
}
