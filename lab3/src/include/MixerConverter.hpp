#ifndef MIXERCONVERTER_H
#define MIXERCONVERTER_H

#include "./WavFile.hpp"
#include <vector>
#include <cstdint>

class MixerConverter {
public:
    bool mix(WavFile& wavFile1, const WavFile& wavFile2, double startSecond, double endSecond);
};

#endif
