#ifndef SILENCECONVERTER_H
#define SILENCECONVERTER_H

#include "./WavFile.hpp"
#include <vector>
#include <cstdint>

class SilenceConverter {
public:
    bool mute(WavFile& wavFile, double startSecond, double endSecond);
};

#endif
