#include "./include/WavFileException.hpp"

WavFileException WavFileException::fileOpenError(const std::string& filename) {
    return WavFileException("\nError opening file: " + filename);
}

WavFileException WavFileException::invalidFormatError() {
    return WavFileException("\nInvalid WAV file format.");
}

WavFileException WavFileException::fileWriteError(const std::string& filename) {
    return WavFileException("\nError writing file: " + filename);
}
