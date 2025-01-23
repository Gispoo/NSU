#include "./include/MixerConverterException.hpp"

MixerConverterException MixerConverterException::fileParametersMismatch() {
    return MixerConverterException("\nFiles must have the same number of channels, bit rate, and sample rate.\n");
}

MixerConverterException MixerConverterException::InvalidMuteRange() {
    return MixerConverterException("\nInvalid mute range.\n");
}

MixerConverterException MixerConverterException::InvalidMixRange() {
    return MixerConverterException("\nInvalid mix range.\n");
}

MixerConverterException MixerConverterException::secondFileSmallerMixRange() {
    return MixerConverterException("The second file is smaller than the mix range");
}