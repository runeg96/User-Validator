#pragma once

#include <cstddef>
#include <string>

#include "app/app_state.hpp"

class UserValidator;

class UserPipeline
{
public:
    /// @brief Parse the Json file containing user data and populate the application state with the parsed users and any rejected records.
    /// @param io_state The current state of the application, which will be updated with the parsed users and any rejected records.
    /// @return True if the parsing was successful, false otherwise.
    bool parse(AppState& io_state) const;

    /// @brief Validate the users in the application state.
    /// @param io_state The current state of the application, which will be updated with the validation results.
    /// @return True if the validation was successful, false otherwise.
    bool validate(AppState& io_state) const;

    /// @brief Export the valid users from the application state.
    /// @param i_state The current state of the application.
    /// @return True if the export was successful, false otherwise.
    bool exportValidUsers(const AppState& i_state) const;

private:
    /// @brief Formats a message with user context information.
    /// @param i_message The message to format.
    /// @param i_userIndex The index of the user, if available.
    /// @param i_userName The name of the user, if available.
    /// @return A formatted message string.
    static std::string formatMessageWithUserContext(const std::string& i_message, std::optional<size_t> i_userIndex = std::nullopt, std::optional<std::string> i_userName = std::nullopt);

    /// @brief Validates a list of users using the provided validator and categorizes them into valid and invalid users.
    /// @param i_users The list of users to validate.
    /// @param i_validator The validator to use for validating the users.
    /// @param o_validUsers The output list that will be populated with valid users.
    /// @param o_invalidUsers The output list that will be populated with invalid users and their
    static void validateUsers(const std::vector<User>& i_users, const UserValidator& i_validator, std::vector<User>& o_validUsers, std::vector<InvalidUser>& o_invalidUsers);
};