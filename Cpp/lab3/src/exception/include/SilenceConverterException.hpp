#ifndef SILENCE_CONVERTER_EXCEPTION_H
#define SILENCE_CONVERTER_EXCEPTION_H

#include <string>
#include <stdexcept>

class SilenceConverterException : public std::runtime_error {
public:
    SilenceConverterException(const std::string& message) : std::runtime_error(message) {}

    static SilenceConverterException InvalidMuteRange();
};

#endif
