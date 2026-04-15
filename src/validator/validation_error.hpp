#pragma once

#include <string>

struct ValidationError
{
    std::string field;
    std::string message;
};