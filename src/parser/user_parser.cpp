#include "user_parser.hpp"

#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>

#include <iostream>
#include <optional>
#include <sstream>
#include <utility>

#include "logging/logEntry.hpp"
#include "logging/logger.hpp"
#include "mapper/user_json_mapper.hpp"
#include "utils/file_utils.hpp"

namespace {

std::optional<std::string> extractUserName(const rapidjson::Value& i_jsonUser)
{
    if (!i_jsonUser.IsObject())
    {
        return std::nullopt;
    }

    if (!i_jsonUser.HasMember("Navn") || !i_jsonUser["Navn"].IsString())
    {
        return std::nullopt;
    }

    return std::string(i_jsonUser["Navn"].GetString());
}

void logRuntimeError(const Logger* i_logger, const std::string& i_stage, const std::string& i_message, std::optional<size_t> i_userIndex = std::nullopt, std::optional<std::string> i_userName = std::nullopt)
{
    if (i_logger != nullptr)
    {
        i_logger->log(LogEntry{ i_stage, "error", std::move(i_userIndex), std::move(i_userName), i_message });
        return;
    }

    std::cerr << i_message << std::endl;
}

} // namespace

bool UserParser::init(const std::string& i_schemaPath)
{
    return loadSchema(i_schemaPath);
}

bool UserParser::loadSchema(const std::string& i_schemaPath)
{
    const std::string schemaString = utils::readFile(i_schemaPath);
    if (schemaString.empty())
    {
        std::cerr << "Failed to read JSON schema file: " << i_schemaPath << std::endl;
        return false;
    }

    m_schemaJson.Parse(schemaString.c_str());
    if (m_schemaJson.HasParseError())
    {
        std::cerr << "Schema parse error: "
                  << rapidjson::GetParseError_En(m_schemaJson.GetParseError())
                  << " at offset " << m_schemaJson.GetErrorOffset() << std::endl;
        return false;
    }

    if (!m_schemaJson.HasMember("items") || !m_schemaJson["items"].IsObject())
    {
        std::cerr << "Schema is missing an object-valued 'items' definition for single-user validation"
                  << std::endl;
        return false;
    }

    m_userSchemaJson.CopyFrom(m_schemaJson["items"], m_userSchemaJson.GetAllocator());

    m_schema     = std::make_unique<rapidjson::SchemaDocument>(m_schemaJson);
    m_userSchema = std::make_unique<rapidjson::SchemaDocument>(m_userSchemaJson);

    return true;
}

// -------------------------------------- Parser public methods --------------------------------------

bool UserParser::parseUserFromFile(const std::string& i_jsonPath, User& io_user) const
{
    const std::string jsonString = utils::readFile(i_jsonPath);
    if (jsonString.empty())
    {
        std::cerr << "Failed to read JSON file: " << i_jsonPath << std::endl;
        return false;
    }

    return parseUser(jsonString, io_user);
}

bool UserParser::parseUser(const std::string& i_jsonString, User& io_user) const
{
    if (!m_userSchema)
    {
        std::cerr << "UserParser is not initialized with a schema." << std::endl;
        return false;
    }

    rapidjson::Document jsonDocument;
    jsonDocument.Parse(i_jsonString.c_str());

    if (jsonDocument.HasParseError())
    {
        std::cerr << "JSON parse error: "
                  << rapidjson::GetParseError_En(jsonDocument.GetParseError())
                  << " at offset " << jsonDocument.GetErrorOffset() << std::endl;
        return false;
    }

    ValidationError schemaError;
    if (!validateSchema(jsonDocument, *m_userSchema, &schemaError))
    {
        std::cerr << "Schema validation failed: " << schemaError.message << std::endl;
        return false;
    }

    return mapUser(jsonDocument, io_user);
}

bool UserParser::parseUsersFromFile(const std::string& i_jsonPath, std::vector<User>& io_users, std::vector<RecordError>& o_recordErrors, const Logger* i_logger) const
{
    const std::string jsonString = utils::readFile(i_jsonPath);
    if (jsonString.empty())
    {
        logRuntimeError(i_logger, "parser", "Failed to read JSON file: " + i_jsonPath);
        return false;
    }

    return parseUsers(jsonString, io_users, o_recordErrors, i_logger);
}

bool UserParser::parseUsers(const std::string& i_jsonString, std::vector<User>& io_users, std::vector<RecordError>& o_recordErrors, const Logger* i_logger) const
{
    io_users.clear();
    o_recordErrors.clear();

    if (!m_userSchema)
    {
        logRuntimeError(i_logger, "parser", "UserParser is not initialized with a schema.");
        return false;
    }

    rapidjson::Document jsonDocument;
    jsonDocument.Parse(i_jsonString.c_str());

    if (jsonDocument.HasParseError())
    {
        std::ostringstream message;
        message << "JSON parse error: "
                << rapidjson::GetParseError_En(jsonDocument.GetParseError())
                << " at offset " << jsonDocument.GetErrorOffset();
        logRuntimeError(i_logger, "parser", message.str());
        return false;
    }

    if (!jsonDocument.IsArray())
    {
        logRuntimeError(i_logger, "parser", "JSON is not an array.");
        return false;
    }

    return mapUsers(jsonDocument, io_users, o_recordErrors, i_logger);
}

// -------------------------------------- Parser private methods --------------------------------------

bool UserParser::mapUser(const rapidjson::Value& i_jsonUser, User& io_user) const
{
    return UserJsonMapper::fromJson(i_jsonUser, io_user);
}

bool UserParser::mapUsers(const rapidjson::Value& i_jsonArray, std::vector<User>& io_users, std::vector<RecordError>& o_recordErrors, const Logger* i_logger) const
{
    io_users.clear();

    if (!i_jsonArray.IsArray())
    {
        std::cerr << "JSON is not an array." << std::endl;
        return false;
    }

    size_t userIndex = 0;
    for (const auto& jsonUser : i_jsonArray.GetArray())
    {
        const std::optional<std::string> userName = extractUserName(jsonUser);

        ValidationError schemaError;
        if (!validateSchema(jsonUser, *m_userSchema, &schemaError))
        {
            RecordError recordError;
            recordError.index = userIndex;
            recordError.name  = userName.value_or("");
            recordError.errors.push_back(schemaError);
            o_recordErrors.push_back(std::move(recordError));

            logRuntimeError(i_logger, "schema", schemaError.field + ": " + schemaError.message, userIndex, userName);
            ++userIndex;
            continue;
        }

        User user;

        if (!mapUser(jsonUser, user))
        {
            ValidationError mappingError{ "record", "Failed to map validated user object." };

            RecordError recordError;
            recordError.index = userIndex;
            recordError.name  = userName.value_or("");
            recordError.errors.push_back(mappingError);
            o_recordErrors.push_back(std::move(recordError));

            logRuntimeError(i_logger, "parser", mappingError.field + ": " + mappingError.message, userIndex, userName);
            ++userIndex;
            continue;
        }

        io_users.push_back(std::move(user));
        ++userIndex;
    }

    return true;
}

// -------------------------------------- Schema validation --------------------------------------

bool UserParser::validateSchema(const rapidjson::Value& i_jsonValue, const rapidjson::SchemaDocument& i_schema, ValidationError* o_error) const
{
    rapidjson::SchemaValidator validator(i_schema);

    if (i_jsonValue.Accept(validator))
    {
        return true;
    }

    rapidjson::StringBuffer documentPath;
    validator.GetInvalidDocumentPointer().StringifyUriFragment(documentPath);

    rapidjson::StringBuffer schemaPath;
    validator.GetInvalidSchemaPointer().StringifyUriFragment(schemaPath);

    std::ostringstream message;
    message << "Document path: " << documentPath.GetString()
            << ", schema path: " << schemaPath.GetString()
            << ", keyword: " << validator.GetInvalidSchemaKeyword();

    if (o_error != nullptr)
    {
        *o_error = ValidationError{ "schema", message.str() };
    }
    else
    {
        std::cerr << "Schema validation failed. " << message.str() << std::endl;
    }

    return false;
}