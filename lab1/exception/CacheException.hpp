#ifndef CACHEEXCEPTION_H
#define CACHEEXCEPTION_H

#include <stdexcept>
#include <string>

class KeyNotFoundException : public std::runtime_error {
public:
  explicit KeyNotFoundException(const std::string& message) : std::runtime_error("KeyNotFoundException: " + message) {}
};

class SmallSizeCacheException : public std::runtime_error {
public:
  explicit SmallSizeCacheException(const std::string& message) : std::runtime_error("SmallSizeCacheException: " + message) {}
};

#endif