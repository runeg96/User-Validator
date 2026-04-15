#pragma once

#include <string>

struct ErrorDetail
{
    std::string field;
    std::string message;

    std::string toString() const
    {
        return field + ": " + message;
    }
};