#include "./include/SilenceConverterException.hpp"

SilenceConverterException SilenceConverterException::InvalidMuteRange() {
    return SilenceConverterException("\nInvalid mute range.");
}