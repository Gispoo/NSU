#include "./include/WavFile.hpp"
#include <iostream>
#include <fstream>
#include <cstring>

bool WavFile::read(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return false;
    }

    // Read header
    file.read(reinterpret_cast<char*>(&header), sizeof(header));
    
    
    // Header validation check (optional)
    if (header.RIFFId != 0x46464952 || header.WAVEId != 0x45564157 || header.FMTId != 0x20746D66 ) {
        std::cerr << "Invalid WAV file format." << std::endl;
        file.close();
        return false;
    }

    // Calculate data size
    int dataSize = header.chunkDataSize - 36;
    data.resize(dataSize);
    
    
    // Read data
    file.read(data.data(), dataSize);

    file.close();
    return true;
}

bool WavFile::write(const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Error creating file: " << filename << std::endl;
            return false;
        }

    // Write header
    file.write(reinterpret_cast<char*>(&header), sizeof(header));

    // Write data
    file.write(data.data(), data.size());
    file.close();
    return true;
}
