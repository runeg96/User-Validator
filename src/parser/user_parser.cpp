#include "user_parser.hpp"

#include "logging/logger.hpp"
#include "mapper/user_json_mapper.hpp"
#include "parser/parser_common.hpp"
#include "utils/utils.hpp"

namespace {

constexpr const char* c_userSchema = R"json(
{
    "type": "object",
    "required": ["Navn", "Alder", "Email", "Spilleplatform", "Oprettelsesdato"],
    "properties": {
        "Navn": { "type": "string", "minLength": 1 },
        "Alder": { "type": "integer", "minimum": 0 },
        "Email": { "type": "string", "minLength": 1 },
        "Spilleplatform": { "type": "string", "minLength": 1 },
        "Oprettelsesdato": { "type": "string", "minLength": 1 },
        "Mobilnummer": { "type": ["string", "null"] },
        "Telefonnummer": { "type": ["string", "null"] }
    },
    "additionalProperties": false
}
)json";

std::string extractUserName(const rapidjson::Value& i_jsonUser)
{
    if (!i_jsonUser.IsObject() || !i_jsonUser.HasMember("Navn") || !i_jsonUser["Navn"].IsString())
    {
        return "";
    }

    return std::string(i_jsonUser["Navn"].GetString());
}

std::string formatMessageWithUserContext(const std::string& i_message, size_t i_userIndex, const std::string& i_userName = "")
{
    std::string formattedMessage;

    formattedMessage += "[user-index=" + std::to_string(i_userIndex) + "] ";

    if (!i_userName.empty())
    {
        formattedMessage += "[user-name=" + i_userName + "] ";
    }

    formattedMessage += i_message;
    return formattedMessage;
}

} // namespace

UserParser::UserParser()
{
    // Parse the user schema
    m_userSchemaJson.Parse(c_userSchema);
    if (m_userSchemaJson.HasParseError())
    {
        std::string message = "Built-in schema parse error: " + std::string(rapidjson::GetParseError_En(m_userSchemaJson.GetParseError())) + " at offset " + std::to_string(m_userSchemaJson.GetErrorOffset());
        LOG_ERROR(LogStage::Parser, message);
        throw std::runtime_error(message);
    }

    if (!m_userSchemaJson.IsObject())
    {
        std::string message = "Built-in schema must be a JSON object.";
        LOG_ERROR(LogStage::Parser, message);
        throw std::runtime_error(message);
    }

    // save the compiled schema document for validation
    m_userSchema = std::make_unique<rapidjson::SchemaDocument>(m_userSchemaJson);
}

// -------------------------------------- Parser public methods --------------------------------------

bool UserParser::parseUserFromFile(const std::string& i_jsonPath, User& io_user) const
{
    const std::string jsonString = utils::readFile(i_jsonPath);
    if (jsonString.empty())
    {
        LOG_ERROR(LogStage::Parser, "Failed to read JSON file: " + i_jsonPath);
        return false;
    }

    return parseUser(jsonString, io_user);
}

bool UserParser::parseUser(const std::string& i_jsonString, User& io_user) const
{
    rapidjson::Document userJson;
    if (!parser_common::parseJsonDocument(i_jsonString, userJson))
    {
        return false;
    }

    return parseUser(userJson, io_user);
}

bool UserParser::parseUsersFromFile(const std::string& i_jsonPath, std::vector<User>& io_users, std::vector<RejectedRecord>& o_rejectedRecords) const
{
    rapidjson::Document jsonDocument;
    if (!parser_common::parseJsonDocumentFromFile(i_jsonPath, jsonDocument))
    {
        return false;
    }

    return parseUsers(jsonDocument, io_users, o_rejectedRecords);
}

bool UserParser::parseUsers(const std::string& i_jsonString, std::vector<User>& io_users, std::vector<RejectedRecord>& o_rejectedRecords) const
{
    rapidjson::Document jsonDocument;
    if (!parser_common::parseJsonDocument(i_jsonString, jsonDocument))
    {
        return false;
    }

    return parseUsers(jsonDocument, io_users, o_rejectedRecords);
}

// -------------------------------------- Parser private methods --------------------------------------

bool UserParser::validateUser(const rapidjson::Value& i_jsonUser, ErrorDetail* o_schemaError) const
{
    if (!parser_common::validateSchema(i_jsonUser, *m_userSchema, o_schemaError))
    {
        if (o_schemaError)
        {
            LOG_ERROR(LogStage::Parser, "Schema validation failed: " + o_schemaError->toString());
        }
        return false;
    }

    return true;
}

bool UserParser::parseUser(const rapidjson::Value& i_jsonValue, User& io_user, ErrorDetail* o_schemaError) const
{
    if (!validateUser(i_jsonValue, o_schemaError))
    {
        if (o_schemaError)
        {
            LOG_ERROR(LogStage::Parser, "Schema validation failed: " + o_schemaError->toString());
        }
        return false;
    }

    return mapUser(i_jsonValue, io_user);
}

bool UserParser::parseUsers(const rapidjson::Value& i_jsonValue, std::vector<User>& io_users, std::vector<RejectedRecord>& o_rejectedRecords) const
{
    io_users.clear();
    o_rejectedRecords.clear();

    if (!i_jsonValue.IsArray())
    {
        LOG_ERROR(LogStage::Parser, "JSON is not an array.");
        return false;
    }

    size_t userIndex = 0;
    for (const auto& jsonUser : i_jsonValue.GetArray())
    {
        User user;
        ErrorDetail schemaError;
        if (!parseUser(jsonUser, user, &schemaError))
        {
            const std::string userName = extractUserName(jsonUser);

            RejectedRecord rejectedRecord;
            rejectedRecord.index = userIndex;
            rejectedRecord.name  = userName;
            rejectedRecord.jsonValue.CopyFrom(jsonUser, rejectedRecord.jsonValue.GetAllocator());
            rejectedRecord.errors.push_back(schemaError);
            o_rejectedRecords.push_back(std::move(rejectedRecord));

            LOG_ERROR(LogStage::Parser, formatMessageWithUserContext("Schema validation failed: " + schemaError.toString(), userIndex, userName));
            ++userIndex;
            continue;
        }

        io_users.push_back(std::move(user));
        ++userIndex;
    }

    return true;
}

bool UserParser::mapUser(const rapidjson::Value& i_jsonUser, User& io_user) const
{
    return UserJsonMapper::fromJson(i_jsonUser, io_user);
}