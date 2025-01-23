#include "./include/MixerConverter.hpp"
#include "./include/WavFile.hpp"
#include "./exception/include/MixerConverterException.hpp"
#include <iostream>
#include <stdexcept>
#include <numeric>
#include <algorithm>

bool MixerConverter::mix(WavFile& wavFile1, const WavFile& wavFile2, double startSecond, double endSecond) {
    if (wavFile1.header.bitsPerSample != wavFile2.header.bitsPerSample 
    || wavFile1.header.numberOfChanels != wavFile2.header.numberOfChanels 
    || wavFile1.header.sampleRate != wavFile2.header.sampleRate) {
        throw MixerConverterException::fileParametersMismatch();
    }
        
    if (startSecond < 0 
    || endSecond > (double)(wavFile1.data.size() / (wavFile1.header.bitsPerSample / 8) / wavFile1.header.numberOfChanels) / wavFile1.header.sampleRate 
    || startSecond >= endSecond) {
        throw MixerConverterException::InvalidMuteRange();
    }

    int bytesPerSample = wavFile1.header.bitsPerSample / 8;
    int numberOfChanels = wavFile1.header.numberOfChanels;
    int startSample = static_cast<int>(startSecond * wavFile1.header.sampleRate);
    int endSample = static_cast<int>(endSecond * wavFile1.header.sampleRate);
    int startByte = startSample * bytesPerSample * numberOfChanels;
    int endByte = endSample * bytesPerSample * numberOfChanels;
    
    if (startByte > wavFile1.data.size() || endByte > wavFile1.data.size() ){
        throw MixerConverterException::InvalidMixRange();
    }
    
    int mixDataSize = endByte - startByte;
    
    if (mixDataSize > wavFile2.data.size()) {
        throw MixerConverterException::secondFileSmallerMixRange();
    }

    for (int i = 0; i < mixDataSize; ++i) {
        int sample1 = 0;
        int sample2 = 0;
        
        if (bytesPerSample == 1) {
            sample1 = static_cast<unsigned char>(wavFile1.data[startByte + i]);
            sample2 = static_cast<unsigned char>(wavFile2.data[i]);
        } else if (bytesPerSample == 2) {
            sample1 = static_cast<short>((static_cast<unsigned char>(wavFile1.data[startByte + i]) | (static_cast<unsigned char>(wavFile1.data[startByte + i+1]) << 8)));
            sample2 = static_cast<short>((static_cast<unsigned char>(wavFile2.data[i]) | (static_cast<unsigned char>(wavFile2.data[i+1]) << 8 )));
        }
            
        int mixedSample = static_cast<int>(sample1 + sample2) / 2 ;
        
        if (bytesPerSample == 1) {
            wavFile1.data[startByte + i] = static_cast<unsigned char>(std::clamp(mixedSample, 0, 255));
        } else if (bytesPerSample == 2) {
            wavFile1.data[startByte + i] = static_cast<unsigned char>(mixedSample & 0xFF);
            wavFile1.data[startByte + i + 1] = static_cast<unsigned char>((mixedSample >> 8) & 0xFF);
        }
        i += bytesPerSample - 1;
    }
    
    return true;
}