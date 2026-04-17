#pragma once

#include <cstddef>
#include <string>

#include "app/app_state.hpp"

class UserValidator;

class UserPipeline
{
public:
    bool parse(AppState& io_state) const;
    bool validate(AppState& io_state) const;
    bool exportValidUsers(const AppState& i_state) const;

private:
    static std::string formatMessageWithUserContext(const std::string& i_message, std::optional<size_t> i_userIndex = std::nullopt, std::optional<std::string> i_userName = std::nullopt);
    static void validateUsers(const std::vector<User>& i_users, const UserValidator& i_validator, std::vector<User>& o_validUsers, std::vector<InvalidUser>& o_invalidUsers);
};