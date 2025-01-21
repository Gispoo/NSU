#ifndef CONFIGPARSER_H
#define CONFIGPARSER_H

#include "Converter.hpp"
#include <vector>
#include <string>
#include <memory>

class ConfigParser {
public:
    ConfigParser(const std::string& configFilename);
    ~ConfigParser() = default;
    std::vector<std::unique_ptr<Converter>> parseConfig();
    bool isValid() const;

private:
    std::string configFilename;
    bool valid = false;
};

#endif
