#include "parser/parser_common.hpp"

#include <rapidjson/error/en.h>
#include <rapidjson/stringbuffer.h>

#include "utils/utils.hpp"

namespace parser_common {

bool parseJsonDocument(const std::string& i_jsonString, rapidjson::Document& o_jsonDocument)
{
    o_jsonDocument.Parse(i_jsonString.c_str());

    if (!o_jsonDocument.HasParseError())
    {
        return true;
    }

    std::string errorMessage = "JSON parse error: " + std::string(rapidjson::GetParseError_En(o_jsonDocument.GetParseError())) + " at offset " + std::to_string(o_jsonDocument.GetErrorOffset());
    LOG_ERROR(LogStage::Parser, errorMessage);
    return false;
}

bool parseJsonDocumentFromFile(const std::string& i_jsonPath, rapidjson::Document& o_jsonDocument)
{
    const std::string jsonString = utils::readFile(i_jsonPath);
    if (jsonString.empty())
    {
        LOG_ERROR(LogStage::Parser, "Failed to read JSON file: " + i_jsonPath);
        return false;
    }

    return parseJsonDocument(jsonString, o_jsonDocument);
}

bool validateSchema(const rapidjson::Value& i_jsonValue, const rapidjson::SchemaDocument& i_schema, ErrorDetail* o_error)
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

    std::string message = "Document path: " + std::string(documentPath.GetString()) + ", schema path: " + std::string(schemaPath.GetString()) + ", keyword: " + std::string(validator.GetInvalidSchemaKeyword());

    if (o_error != nullptr)
    {
        *o_error = ErrorDetail{ "schema", message };
        return false;
    }

    LOG_ERROR(LogStage::Schema, "Schema validation failed: " + message);
    return false;
}

} // namespace parser_common