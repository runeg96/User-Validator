#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "errors/error_detail.hpp"

struct RejectedRecord
{
    std::size_t index = 0;
    std::string name;
    std::vector<ErrorDetail> errors;
};