#ifndef WAVFILE_H
#define WAVFILE_H

#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>

class WavFile {
public:
    WavFile(const std::string& filename);
    ~WavFile() {}

    bool isValid() const;
    int getSampleRate() const;
    int getBitsPerSample() const;
    int getNumChannels() const;
    std::vector<int16_t> getSamples() const;
    bool save(const std::string& filename, const std::vector<int16_t>& samples);

private:
    struct WavHeader {
        char chunkId[4];
        uint32_t chunkSize;
        char format[4];
        char subChunk1Id[4];
        char subChunk2Id[4];
        uint32_t subChunk1Size;
        uint16_t audioFormat;
        uint16_t numChannels;
        uint32_t sampleRate;
        uint32_t byteRate;
        uint16_t blockAlign;
        uint16_t bitsPerSample;
        uint32_t subChunk2Size;
    } header;

    std::string filename;
    std::ifstream file;
    std::vector<int16_t> samples;
    bool valid = false;

    bool parseHeader();
    bool readData();
    bool writeHeader(std::ofstream& file, int numSamples) const;
};

#endif
