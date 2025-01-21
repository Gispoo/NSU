#include "./include/ConfigParser.hpp"
#include "./include/ConverterFactory.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

ConfigParser::ConfigParser(const std::string& configFilename) : configFilename(configFilename) {
    std::ifstream file(configFilename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open config file: " << configFilename << std::endl;
        return;
    }
    valid = true;
}

bool ConfigParser::isValid() const {
    return valid;
}

std::vector<std::unique_ptr<Converter>> ConfigParser::parseConfig() {
    std::vector<std::unique_ptr<Converter>> converters;
    std::ifstream file(configFilename);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string converterType;
        if (!(iss >> converterType)) {
          continue;
        }

        try {
            std::unique_ptr<Converter> converter = ConverterFactory::createConverter(converterType);
            std::vector<std::string> params;
            std::string param;
              while (iss >> param) {
                params.push_back(param);
            }
            converter->setParameters(params);
            converters.push_back(std::move(converter));
        }
        catch (const std::runtime_error& e) {
              std::cerr << "Error: in config file " << e.what() << std::endl;
              continue;
        }
    }

    return converters;
}
