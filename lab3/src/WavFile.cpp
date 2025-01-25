#include "./include/WavFile.hpp"
#include "./include/config.hpp"
#include "./exception/include/WavFileException.hpp"
#include <iostream>
#include <fstream>
#include <cstring>

bool WavFile::read(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw WavFileException::fileOpenError(filename);
    }

    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    if (header.RIFFId != RIFF || header.WAVEId != WAVE || header.FMTId != FMT) {
        file.close();
        throw WavFileException::invalidFormatError();
    }

    int dataSize = header.chunkDataSize - SIZE_TITLE;
    data.resize(dataSize);

    file.read(data.data(), dataSize);

    file.close();
    return true;
}

bool WavFile::write(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw WavFileException::fileWriteError(filename);
    }

    file.write(reinterpret_cast<char*>(&header), sizeof(header));

    file.write(data.data(), data.size());
    file.close();
    return true;
}
