#pragma once

#include <vector>

#include "errors/error_detail.hpp"
#include "user.hpp"

struct InvalidUser
{
    User user;
    std::vector<ErrorDetail> errors;
};