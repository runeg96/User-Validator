#pragma once

#include <memory>
#include <string>
#include <vector>

// rapidjson
#include <rapidjson/document.h>
#include <rapidjson/schema.h>

#include "user.hpp"
#include "validator/record_error.hpp"
#include "validator/validation_error.hpp"

class Logger;

class UserParser
{
public:
    /// @brief Initializes the parser with a JSON schema.
    /// @param i_schemaPath The path to the JSON schema file.
    /// @return True if initialization was successful, false otherwise.
    bool init(const std::string& i_schemaPath);

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
    /// @param o_recordErrors The vector to populate with parser/schema errors for skipped records.
    /// @param i_logger Optional logger used to emit runtime parse errors.
    /// @return True if parsing and validation were successful, false otherwise.
    bool parseUsers(const std::string& i_jsonString, std::vector<User>& io_users, std::vector<RecordError>& o_recordErrors, const Logger* i_logger = nullptr) const;

    /// @brief Parses multiple users from a JSON file.
    /// @param i_jsonPath The path to the JSON file containing user data.
    /// @param io_users The vector to populate with parsed User objects.
    /// @param o_recordErrors The vector to populate with parser/schema errors for skipped records.
    /// @param i_logger Optional logger used to emit runtime parse errors.
    /// @return True if parsing and validation were successful, false otherwise.
    bool parseUsersFromFile(const std::string& i_jsonPath, std::vector<User>& io_users, std::vector<RecordError>& o_recordErrors, const Logger* i_logger = nullptr) const;

private:
    /// @brief Loads the JSON schema from a file.
    /// @param i_schemaPath The path to the JSON schema file.
    /// @return True if the schema was loaded successfully, false otherwise.
    bool loadSchema(const std::string& i_schemaPath);

    /// @brief Maps a JSON object to a User object.
    /// @param i_jsonUser The JSON object containing user data.
    /// @param io_user The User object to populate with parsed data.
    /// @return True if mapping was successful, false otherwise.
    bool mapUser(const rapidjson::Value& i_jsonUser, User& io_user) const;

    /// @brief Maps a JSON array to a vector of User objects.
    /// @param i_jsonArray The JSON array containing user data.
    /// @param io_users The vector to populate with parsed User objects.
    /// @param o_recordErrors The vector to populate with parser/schema errors for skipped records.
    /// @param i_logger Optional logger used to emit runtime parse errors.
    /// @return True if mapping was successful, false otherwise.
    bool mapUsers(const rapidjson::Value& i_jsonArray, std::vector<User>& io_users, std::vector<RecordError>& o_recordErrors, const Logger* i_logger) const;

    /// @brief Validates a JSON value against a JSON schema.
    /// @param i_jsonValue The JSON value to validate.
    /// @param i_schema The JSON schema to validate against.
    /// @param o_error Optional output for schema validation details.
    /// @return True if validation was successful, false otherwise.
    bool validateSchema(const rapidjson::Value& i_jsonValue, const rapidjson::SchemaDocument& i_schema, ValidationError* o_error = nullptr) const;

    /// @brief Helper function that splits a comma-separated string of platforms into a vector of strings.
    /// @param i_platforms The comma-separated string of platforms.
    /// @return A vector of individual platform strings.
    static std::vector<std::string> splitPlatforms(const std::string& i_platforms);

    rapidjson::Document m_schemaJson;
    rapidjson::Document m_userSchemaJson;
    std::unique_ptr<rapidjson::SchemaDocument> m_schema;
    std::unique_ptr<rapidjson::SchemaDocument> m_userSchema;
};