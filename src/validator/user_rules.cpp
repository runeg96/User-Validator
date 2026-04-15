#include "validator/user_rules.hpp"

#include <regex>

void EmailValidator::validate(const User& i_user, std::vector<ValidationError>& io_errors) const
{
    static const std::regex emailPattern(
        R"(^[A-Za-z0-9._%+\-]+@[A-Za-z0-9.\-]+\.[A-Za-z]{2,}$)");

    if (!std::regex_match(i_user.email, emailPattern))
    {
        io_errors.push_back({ "Email", "Invalid email format" });
    }
}

void ContactValidator::validate(const User& i_user, std::vector<ValidationError>& io_errors) const
{
    const bool hasMobile = i_user.mobileNumber.has_value() && !i_user.mobileNumber->empty();
    const bool hasPhone  = i_user.phoneNumber.has_value() && !i_user.phoneNumber->empty();

    if (!hasMobile && !hasPhone)
    {
        io_errors.push_back({ "Mobilnummer/Telefonnummer",
                              "Either mobile number or phone number must be provided" });
    }
}

void PlatformsValidator::validate(const User& i_user, std::vector<ValidationError>& io_errors) const
{
    if (i_user.platforms.size() < 2)
    {
        io_errors.push_back({ "Spilleplatform",
                              "User must have at least 2 favorite platforms" });
    }
}