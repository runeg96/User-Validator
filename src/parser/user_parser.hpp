#pragma once

#include <memory>
#include <string>
#include <vector>

// rapidjson
#include <rapidjson/document.h>
#include <rapidjson/schema.h>

#include "errors/error_detail.hpp"
#include "errors/rejected_record.hpp"
#include "user.hpp"

class UserParser
{
public:
    /// @brief Initializes the parser with the built-in JSON schema.
    UserParser();

    /// @brief Parses a single user from a JSON string.
    /// @param i_jsonString The JSON string containing user data.
    /// @param io_user The User object to populate with parsed data.
    /// @return True if parsing and validation were successful, false otherwise.
    bool parseUser(const std::string& i_jsonString, User& io_user) const;

    /// @brief Parses a single user from a JSON file.
    /// @param i_jsonPath The path to the JSON file containing user data.
    /// @param io_user The User object to populate with parsed data.
    /// @return True if parsing and validation were successful, false otherwise.
    bool parseUserFromFile(const std::string& i_jsonPath, User& io_user) const;

    /// @brief Parses multiple users from a JSON string.
    /// @param i_jsonString The JSON string containing user data.
    /// @param io_users The vector to populate with parsed User objects.
    /// @param o_rejectedRecords The vector to populate with parser/schema issues for skipped records.
    /// @return True if parsing and validation were successful, false otherwise.
    bool parseUsers(const std::string& i_jsonString, std::vector<User>& io_users, std::vector<RejectedRecord>& o_rejectedRecords) const;

    /// @brief Parses multiple users from a JSON file.
    /// @param i_jsonPath The path to the JSON file containing user data.
    /// @param io_users The vector to populate with parsed User objects.
    /// @param o_rejectedRecords The vector to populate with parser/schema issues for skipped records.
    /// @return True if parsing and validation were successful, false otherwise.
    bool parseUsersFromFile(const std::string& i_jsonPath, std::vector<User>& io_users, std::vector<RejectedRecord>& o_rejectedRecords) const;

private:
    /// @brief Validates a JSON object against the user schema.
    /// @param i_jsonUser The JSON object containing user data.
    /// @param o_schemaError The ErrorDetail object to populate with schema validation errors.
    /// @return True if the JSON object is valid, false otherwise.
    bool validateUser(const rapidjson::Value& i_jsonUser, ErrorDetail* o_schemaError) const;

    /// @brief Parses a JSON value into a User object and validates it against the schema.
    /// @param i_jsonValue The JSON value containing user data.
    /// @param io_user The User object to populate with parsed data.
    /// @param o_schemaError The ErrorDetail object to populate with schema validation errors.
    /// @return True if parsing and validation were successful, false otherwise.
    bool parseUser(const rapidjson::Value& i_jsonValue, User& io_user, ErrorDetail* o_schemaError = nullptr) const;

    /// @brief Parses a JSON array into multiple User objects and validates each against the schema.
    /// @param i_jsonValue The JSON value containing an array of user data.
    /// @param io_users The vector to populate with parsed User objects.
    /// @param o_rejectedRecords The vector to populate with parser/schema issues for skipped records.
    /// @return True if parsing and validation were successful, false otherwise.
    bool parseUsers(const rapidjson::Value& i_jsonValue, std::vector<User>& io_users, std::vector<RejectedRecord>& o_rejectedRecords) const;

    /// @brief Maps a JSON object to a User object.
    /// @param i_jsonUser The JSON object containing user data.
    /// @param io_user The User object to populate with parsed data.
    /// @return True if mapping was successful, false otherwise.
    bool mapUser(const rapidjson::Value& i_jsonUser, User& io_user) const;

    rapidjson::Document m_userSchemaJson;
    std::unique_ptr<rapidjson::SchemaDocument> m_userSchema;
};