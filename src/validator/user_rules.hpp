#pragma once

#include "validator/base_validator.hpp"

/// @brief Validator for checking if a user's email is in a valid format.
class EmailValidator : public BaseValidator
{
public:
    void validate(const User& i_user, std::vector<ErrorDetail>& io_errors) const override;
};

/// @brief Validator for ensuring that a user has at least one contact method (mobile or phone).
class ContactValidator : public BaseValidator
{
public:
    void validate(const User& i_user, std::vector<ErrorDetail>& io_errors) const override;
};

/// @brief Validator for ensuring that a user has at least one platform specified.
class PlatformsValidator : public BaseValidator
{
public:
    void validate(const User& i_user, std::vector<ErrorDetail>& io_errors) const override;
};