#ifndef CONVERTERFACTORY_H
#define CONVERTERFACTORY_H

#include "Converter.hpp"
#include <string>
#include <memory>

class ConverterFactory {
public:
    static std::unique_ptr<Converter> createConverter(const std::string& type);
};

#endif
