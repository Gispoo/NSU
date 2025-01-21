#include "./include/ConverterFactory.hpp"
#include "./include/SilenceConverter.hpp"
#include "./include/MixerConverter.hpp"
#include "./include/CustomConverter.hpp"
#include <stdexcept>

std::unique_ptr<Converter> ConverterFactory::createConverter(const std::string& type) {
    if (type == "silence") {
        return std::make_unique<SilenceConverter>();
    }
    else if (type == "mixer") {
        return std::make_unique<MixerConverter>();
    }
     else if (type == "custom") {
        return std::make_unique<CustomConverter>();
    }
    else {
        throw std::runtime_error("Unknown converter type: " + type);
    }
}
