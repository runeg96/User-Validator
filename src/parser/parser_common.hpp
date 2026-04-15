#pragma once

#include <rapidjson/document.h>
#include <rapidjson/schema.h>

#include <string>

#include "errors/error_detail.hpp"
#include "logging/logger.hpp"

namespace parser_common {
/// @brief Parses a JSON string into a rapidjson::Document.
/// @param i_jsonString The JSON string to parse.
/// @param o_jsonDocument The rapidjson::Document to populate with parsed data.
/// @return True if parsing was successful, false otherwise.
bool parseJsonDocument(const std::string& i_jsonString, rapidjson::Document& o_jsonDocument);

/// @brief Parses a JSON file into a rapidjson::Document.
/// @param i_jsonPath The path to the JSON file to parse.
/// @param o_jsonDocument The rapidjson::Document to populate with parsed data.
/// @return True if parsing was successful, false otherwise.
bool parseJsonDocumentFromFile(const std::string& i_jsonPath, rapidjson::Document& o_jsonDocument);

/// @brief Validates a JSON value against a schema.
/// @param i_jsonValue The JSON value to validate.
/// @param i_schema The schema to validate against.
/// @param o_error The ErrorDetail object to populate with validation errors.
/// @return True if the JSON value is valid, false otherwise.
bool validateSchema(const rapidjson::Value& i_jsonValue, const rapidjson::SchemaDocument& i_schema, ErrorDetail* o_error = nullptr);

} // namespace parser_common