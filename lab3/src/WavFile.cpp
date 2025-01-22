#include "./include/WavFile.hpp"
#include <iostream>
#include <fstream>
#include <cstring>

WavFile::WavFile(const std::string& filename) : filename(filename), 
    file(filename, std::ios::binary) {
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return;
    }

    if (!parseHeader() || !readData()) {
        std::cerr << "Error: Could not parse file " << filename << std::endl;
        return;
    }

    if (header.audioFormat != 1 || header.numChannels != 1 || header.bitsPerSample != 16 || header.sampleRate != 44100) {
        std::cerr << "Error: Unsupported WAV format. Must be PCM, 1 channel, 16-bit, 44100 Hz." << std::endl;
        return;
    }
    valid = true;
}

bool WavFile::isValid() const {
    return valid;
}

int WavFile::getSampleRate() const {
    return header.sampleRate;
}

int WavFile::getBitsPerSample() const {
    return header.bitsPerSample;
}

int WavFile::getNumChannels() const {
    return header.numChannels;
}

std::vector<int16_t> WavFile::getSamples() const {
    return samples;
}

bool WavFile::parseHeader() {
    file.read(reinterpret_cast<char*>(&header), sizeof(header));

    if (std::strncmp(header.chunkId, "RIFF", 4) != 0 ||
        std::strncmp(header.format, "WAVE", 4) != 0 ||
        std::strncmp(header.subChunk1Id, "fmt ", 4) != 0 ||
        std::strncmp(header.subChunk2Id, "data", 4) != 0) {
        return false;
    }

    return true;
}

bool WavFile::readData() {
    file.seekg(sizeof(header), std::ios::beg);

    int numSamples = header.subChunk2Size / (header.bitsPerSample / 8);
    samples.resize(numSamples);
    file.read(reinterpret_cast<char*>(samples.data()), header.subChunk2Size);

    return true;
}

bool WavFile::save(const std::string& filename, const std::vector<int16_t>& samples) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return false;
    }
    int numSamples = samples.size();
    if (!writeHeader(file, numSamples)) {
        std::cerr << "Error: Could not write WAV header." << std::endl;
        return false;
    }
    file.write(reinterpret_cast<const char*>(samples.data()), numSamples * sizeof(int16_t));

    return true;
}
bool WavFile::writeHeader(std::ofstream& file, int numSamples) const {
    WavHeader newHeader = header;
    newHeader.subChunk2Size = numSamples * (newHeader.bitsPerSample / 8);
    newHeader.chunkSize = 36 + newHeader.subChunk2Size;
    file.write(reinterpret_cast<const char*>(&newHeader), sizeof(newHeader));

    return true;
}
