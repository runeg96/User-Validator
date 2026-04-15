#pragma once

#include <optional>
#include <string>
#include <vector>

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