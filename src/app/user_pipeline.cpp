#include "app/user_pipeline.hpp"

#include <chrono>

#include "exporter/export_service.hpp"
#include "logging/logger.hpp"
#include "parser/user_parser.hpp"
#include "validator/user_validator.hpp"

std::string UserPipeline::formatMessageWithUserContext(const std::string& i_message, std::optional<size_t> i_userIndex, std::optional<std::string> i_userName)
{
    std::string formattedMessage;

    if (i_userIndex.has_value())
    {
        formattedMessage += "[user-index=" + std::to_string(*i_userIndex) + "] ";
    }

    if (i_userName.has_value() && !i_userName->empty())
    {
        formattedMessage += "[user-name=" + *i_userName + "] ";
    }

    formattedMessage += i_message;
    return formattedMessage;
}

void UserPipeline::validateUsers(const std::vector<User>& i_users, const UserValidator& i_validator, std::vector<User>& o_validUsers, std::vector<InvalidUser>& o_invalidUsers)
{
    for (size_t userIndex = 0; userIndex < i_users.size(); ++userIndex)
    {
        const auto& user                      = i_users[userIndex];
        const std::vector<ErrorDetail> errors = i_validator.validate(user);

        if (errors.empty())
        {
            o_validUsers.push_back(user);
        }
        else
        {
            o_invalidUsers.push_back({ user, errors });

            for (const auto& error : errors)
            {
                LOG_ERROR(LogStage::Validator, formatMessageWithUserContext(error.field + ": " + error.message, userIndex, user.name));
            }
        }
    }
}

bool UserPipeline::parse(AppState& io_state) const
{
    UserParser parser;
    LOG_INFO(LogStage::Parser, "Initialized parser with schema.");

    LOG_INFO(LogStage::Parser, "Parsing users from " + io_state.inputJsonPath.string());

    const auto parseStartTime = std::chrono::steady_clock::now();

    const bool parsed = parser.parseUsersFromFile(io_state.inputJsonPath.string(), io_state.users, io_state.rejectedRecords);

    const auto parseEndTime = std::chrono::steady_clock::now();

    if (!parsed)
    {
        LOG_ERROR(LogStage::Parser, "Failed to parse users from: " + io_state.inputJsonPath.string());
        return false;
    }

    io_state.parseDurationUs = std::chrono::duration_cast<std::chrono::microseconds>(parseEndTime - parseStartTime).count();

    LOG_INFO(LogStage::Parser, "Parsed " + std::to_string(io_state.users.size()) + " users and skipped " + std::to_string(io_state.rejectedRecords.size()) + " records during parser/schema stage.");

    return true;
}

bool UserPipeline::validate(AppState& io_state) const
{
    UserValidator validator;

    LOG_INFO(LogStage::Validator, "Validating " + std::to_string(io_state.users.size()) + " parsed users.");

    const auto validationStartTime = std::chrono::steady_clock::now();
    validateUsers(io_state.users, validator, io_state.validUsers, io_state.invalidUsers);
    const auto validationEndTime = std::chrono::steady_clock::now();

    io_state.validationDurationUs = std::chrono::duration_cast<std::chrono::microseconds>(validationEndTime - validationStartTime).count();

    LOG_INFO(LogStage::Validator, "Validation produced " + std::to_string(io_state.validUsers.size()) + " valid users and " + std::to_string(io_state.invalidUsers.size()) + " invalid users.");

    return true;
}

bool UserPipeline::exportValidUsers(const AppState& i_state) const
{
    ExportService exportService;
    if (exportService.exportUsers(i_state.validUsers, i_state.jsonExportPath.string()))
    {
        LOG_INFO(LogStage::Export, "Successfully exported valid users to: " + i_state.jsonExportPath.string());
    }
    else
    {
        LOG_ERROR(LogStage::Export, "Failed to export valid users to: " + i_state.jsonExportPath.string());
        return false;
    }

    if (exportService.exportUsers(i_state.validUsers, i_state.xmlExportPath.string()))
    {
        LOG_INFO(LogStage::Export, "Successfully exported valid users to: " + i_state.xmlExportPath.string());
    }
    else
    {
        LOG_ERROR(LogStage::Export, "Failed to export valid users to: " + i_state.xmlExportPath.string());
        return false;
    }

    return true;
}