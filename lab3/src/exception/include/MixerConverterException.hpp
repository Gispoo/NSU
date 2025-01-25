#ifndef MIXER_CONVERTER_EXCEPTION_H
#define MIXER_CONVERTER_EXCEPTION_H

#include <string>
#include <stdexcept>

class MixerConverterException : public std::runtime_error {
public:
    MixerConverterException(const std::string& message) : std::runtime_error(message) {}

    static MixerConverterException fileParametersMismatch();
    static MixerConverterException InvalidMuteRange();
    static MixerConverterException InvalidMixRange();
    static MixerConverterException secondFileSmallerMixRange();
};

#endif
