#include "./include/SilenceConverter.hpp"
#include "./include/WavFile.hpp"
#include <iostream>
#include <stdexcept>
#include <cstdlib>

bool SilenceConverter::mute(WavFile& wavFile, double startSecond, double endSecond) {
    if (startSecond < 0 || endSecond > (double)(wavFile.data.size() / (wavFile.header.bitsPerSample/8) / wavFile.header.numberOfChanels) / wavFile.header.sampleRate || startSecond >= endSecond) {
        std::cerr << "Invalid mute range." << std::endl;
        return false;
    }

    int bytesPerSample = wavFile.header.bitsPerSample / 8;
    int numberOfChanels = wavFile.header.numberOfChanels;
    int startSample = static_cast<int>(startSecond * wavFile.header.sampleRate);
    int endSample = static_cast<int>(endSecond * wavFile.header.sampleRate);
    int startByte = startSample * bytesPerSample * numberOfChanels;
    int endByte = endSample * bytesPerSample * numberOfChanels;
    
    if (startByte > wavFile.data.size() || endByte > wavFile.data.size()) {
        std::cerr << "Invalid mute range." << std::endl;
        return false;
    }
        
    for (int i = startByte; i < endByte; i++) {
        wavFile.data[i] = 0;
    }
    
    return true;
}
