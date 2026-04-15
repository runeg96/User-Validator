#pragma once

#include <string>
#include <vector>

#include "validator/validation_error.hpp"

struct RecordError
{
    size_t index = 0;
    std::string name;
    std::vector<ValidationError> errors;
};