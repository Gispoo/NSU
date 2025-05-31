#ifndef EFIBONACCI_H
#define EFIBONACCI_H

#include <stdexcept>
#include <string>

class NumFibLessZero : public std::runtime_error {
public:
  explicit NumFibLessZero(const std::string& message) : std::runtime_error("NumFibLessZero: " + message) {}
};

#endif