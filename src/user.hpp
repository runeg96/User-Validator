#pragma once

#include <optional>
#include <string>
#include <vector>

#include "validator/validation_error.hpp"

struct User
{
    std::string name;
    int age{};
    std::string email;
    std::vector<std::string> platforms;
    std::string createdAt;
    std::optional<std::string> mobileNumber;
    std::optional<std::string> phoneNumber;
};

struct InvalidUser
{
    User user;
    std::vector<ValidationError> errors;
};