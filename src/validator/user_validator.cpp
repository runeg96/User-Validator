#include "validator/user_validator.hpp"

#include "validator/user_rules.hpp"

UserValidator::UserValidator()
{
    // add all validators here
    addValidator(std::make_unique<EmailValidator>());
    addValidator(std::make_unique<ContactValidator>());
    addValidator(std::make_unique<PlatformsValidator>());
}

void UserValidator::addValidator(std::unique_ptr<BaseValidator> i_validator)
{
    m_validators.push_back(std::move(i_validator));
}

std::vector<ValidationError> UserValidator::validate(const User& i_user) const
{
    std::vector<ValidationError> errors;

    for (const auto& validator : m_validators)
    {
        validator->validate(i_user, errors);
    }

    return errors;
}