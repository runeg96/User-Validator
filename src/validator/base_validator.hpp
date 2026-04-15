#pragma once

#include <vector>

#include "user.hpp"
#include "validator/validation_error.hpp"

class BaseValidator
{
public:
    /// @brief Validates a user and appends any errors to io_errors. This is a pure virtual function that must be implemented by derived classes to perform specific validation checks on the User object.
    /// @param i_user The user to validate.
    /// @param io_errors A vector to which validation errors will be appended.
    virtual void validate(const User& i_user, std::vector<ValidationError>& io_errors) const = 0;
};