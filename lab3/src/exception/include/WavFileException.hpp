#ifndef WAV_FILE_EXCEPTION_H
#define WAV_FILE_EXCEPTION_H

#include <string>
#include <stdexcept>

class WavFileException : public std::runtime_error {
public:
    WavFileException(const std::string& message) : std::runtime_error(message) {}

    static WavFileException fileOpenError(const std::string& filename);
    static WavFileException invalidFormatError();
    static WavFileException fileWriteError(const std::string& filename);
};

#endif
