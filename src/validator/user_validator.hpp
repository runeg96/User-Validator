#pragma once

#include <memory>
#include <vector>

#include "errors/error_detail.hpp"
#include "validator/base_validator.hpp"

class UserValidator
{
public:
    /// @brief Constructs a UserValidator and initializes it with default validation rules. The constructor adds instances of specific validators (e.g., EmailValidator, ContactValidator, PlatformsValidator) to the internal list of validators that will be used to validate User objects.
    UserValidator();

    /// @brief Adds a custom validator to the UserValidator. This allows for extending the validation logic by providing additional validation rules encapsulated in classes derived from BaseValidator.
    /// @param i_validator A unique pointer to the custom validator to add.
    void addValidator(std::unique_ptr<BaseValidator> i_validator);

    /// @brief Validates a User object against all registered validators and returns a vector of ErrorDetail objects representing any validation issues found. Each validator will be applied to the User object, and any issues will be collected and returned as a list.
    /// @param i_user The User object to validate.
    /// @return A vector of ErrorDetail objects containing details about any validation issues that were found
    std::vector<ErrorDetail> validate(const User& i_user) const;

private:
    std::vector<std::unique_ptr<BaseValidator>> m_validators;
};