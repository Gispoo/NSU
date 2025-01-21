#include "./include/CustomConverter.hpp"
#include <iostream>
#include <stdexcept>
#include <algorithm>

CustomConverter::CustomConverter() {}

void CustomConverter::setParameters(const std::vector<std::string>& params) {
    if (params.size() != 1) {
        throw std::runtime_error("CustomConverter: Requires one parameter: gain_factor.");
    }

    try {
        gainFactor = std::stod(params[0]);
    }
    catch (const std::invalid_argument& e) {
        throw std::runtime_error("CustomConverter: Invalid format of argument. Gain factor must be a number");
    }

    if (gainFactor < 0 ) {
          throw std::runtime_error("CustomConverter: Invalid value of argument. Gain factor must be a positive number");
    }

}

std::vector<int16_t> CustomConverter::process(const std::vector<int16_t>& input) {
      std::vector<int16_t> output = input;
       for (int i = 0; i < output.size(); i++){
         int32_t temp = static_cast<int32_t>(output[i]) * gainFactor;
         if (temp > INT16_MAX) {
            output[i] = INT16_MAX;
         } else if (temp < INT16_MIN) {
              output[i] = INT16_MIN;
         } else{
            output[i] = static_cast<int16_t>(temp);
         }

        }
    return output;
}
