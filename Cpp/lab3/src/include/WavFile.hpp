#ifndef WAVFILE_H
#define WAVFILE_H

#include <fstream>
#include <vector>
#include <cstdint>
#include <iostream>

struct WavHeader {
    int RIFFId;
    int chunkDataSize;
    int WAVEId;
    int FMTId;
    int subChunkDataSize;
    short compresionCode;
    short numberOfChanels;
    int sampleRate;
    int avBytesPerSec;
    short blockAlign;
    short bitsPerSample;
    char LISTId[4];
    int subChunk2DataSize;
    char INFOId[4];
    int INFOVal;
};

class WavFile {
public:
    WavHeader header;
    std::vector<char> data;

    bool read(const std::string& filename);
    bool write(const std::string& filename);
};

#endif
