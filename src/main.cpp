#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "errors/invalid_user.hpp"
#include "errors/rejected_record.hpp"
#include "exporter/export_service.hpp"
#include "logging/logEntry.hpp"
#include "logging/logger.hpp"
#include "parser/user_parser.hpp"
#include "user.hpp"
#include "validator/user_validator.hpp"

namespace {

struct AppPaths
{
    std::filesystem::path inputJsonPath;
    std::filesystem::path jsonExportPath;
    std::filesystem::path xmlExportPath;
    std::filesystem::path logPath;
};

struct ValidationResult
{
    std::vector<User> validUsers;
    std::vector<InvalidUser> invalidUsers;
};

struct AppState
{
    AppPaths paths;
    std::vector<User> users;
    std::vector<RejectedRecord> rejectedRecords;
    ValidationResult validationResult;
    long long parseDurationUs      = 0;
    long long validationDurationUs = 0;
};

std::string formatMessageWithUserContext(const std::string& i_message, std::optional<size_t> i_userIndex = std::nullopt, std::optional<std::string> i_userName = std::nullopt)
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

std::filesystem::path getExecutableDir(const char* i_argv0)
{
    return std::filesystem::canonical(i_argv0).parent_path();
}

AppPaths resolvePaths(int i_argc, char* i_argv[])
{
    const std::filesystem::path exeDir = getExecutableDir(i_argv[0]);

    AppPaths paths;
    paths.inputJsonPath  = i_argc > 1 ? std::filesystem::path(i_argv[1]) : exeDir / "users.json";
    paths.jsonExportPath = exeDir / "export" / "users_export.json";
    paths.xmlExportPath  = exeDir / "export" / "users_export.xml";
    paths.logPath        = exeDir / "logs" / "parser.log";

    return paths;
}

ValidationResult validateUsers(const std::vector<User>& i_users, const UserValidator& i_validator)
{
    ValidationResult result;

    for (size_t userIndex = 0; userIndex < i_users.size(); ++userIndex)
    {
        const auto& user                      = i_users[userIndex];
        const std::vector<ErrorDetail> errors = i_validator.validate(user);

        if (errors.empty())
        {
            result.validUsers.push_back(user);
        }
        else
        {
            result.invalidUsers.push_back({ user, errors });

            for (const auto& error : errors)
            {
                LOG_ERROR(LogStage::Validator, formatMessageWithUserContext(error.field + ": " + error.message, userIndex, user.name));
            }
        }
    }

    return result;
}

void printSummary(const AppState& i_state)
{
    std::cout << "Parsed " << i_state.users.size() << " users\n";
    std::cout << "Skipped during parser/schema: " << i_state.rejectedRecords.size() << "\n";
    std::cout << "Valid users: " << i_state.validationResult.validUsers.size() << "\n";
    std::cout << "Invalid users: " << i_state.validationResult.invalidUsers.size() << "\n";
    std::cout << "Parsing took " << i_state.parseDurationUs << " us\n";
    std::cout << "Validation took " << i_state.validationDurationUs << " us\n";
}

bool runParser(AppState& io_state)
{
    UserParser parser;
    LOG_INFO(LogStage::Parser, "Initialized parser with built-in schema.");

    LOG_INFO(LogStage::Parser, "Parsing users from " + io_state.paths.inputJsonPath.string());

    const auto parseStartTime = std::chrono::steady_clock::now();
    const bool parsed         = parser.parseUsersFromFile(io_state.paths.inputJsonPath.string(), io_state.users, io_state.rejectedRecords);
    const auto parseEndTime   = std::chrono::steady_clock::now();

    if (!parsed)
    {
        LOG_ERROR(LogStage::Parser, "Failed to parse users from: " + io_state.paths.inputJsonPath.string());
        return false;
    }

    io_state.parseDurationUs = std::chrono::duration_cast<std::chrono::microseconds>(parseEndTime - parseStartTime).count();

    LOG_INFO(LogStage::Parser, "Parsed " + std::to_string(io_state.users.size()) + " users and skipped " + std::to_string(io_state.rejectedRecords.size()) + " records during parser/schema stage.");

    return true;
}

bool runValidation(AppState& io_state)
{
    UserValidator validator;

    LOG_INFO(LogStage::Validator, "Validating " + std::to_string(io_state.users.size()) + " parsed users.");

    const auto validationStartTime = std::chrono::steady_clock::now();
    io_state.validationResult      = validateUsers(io_state.users, validator);
    const auto validationEndTime   = std::chrono::steady_clock::now();

    io_state.validationDurationUs = std::chrono::duration_cast<std::chrono::microseconds>(validationEndTime - validationStartTime).count();

    LOG_INFO(LogStage::Validator, "Validation produced " + std::to_string(io_state.validationResult.validUsers.size()) + " valid users and " + std::to_string(io_state.validationResult.invalidUsers.size()) + " invalid users.");

    return true;
}

bool runExporter(const AppState& i_state)
{
    ExportService exportService;
    if (exportService.exportUsers(i_state.validationResult.validUsers, i_state.paths.jsonExportPath.string()))
    {
        LOG_INFO(LogStage::Export, "Successfully exported valid users to: " + i_state.paths.jsonExportPath.string());
    }
    else
    {
        LOG_ERROR(LogStage::Export, "Failed to export valid users to: " + i_state.paths.jsonExportPath.string());
        return false;
    }

    if (exportService.exportUsers(i_state.validationResult.validUsers, i_state.paths.xmlExportPath.string()))
    {
        LOG_INFO(LogStage::Export, "Successfully exported valid users to: " + i_state.paths.xmlExportPath.string());
    }
    else
    {
        LOG_ERROR(LogStage::Export, "Failed to export valid users to: " + i_state.paths.xmlExportPath.string());
        return false;
    }

    return true;
}

} // namespace

int main(int argc, char* argv[])
{
    AppState state;
    state.paths = resolvePaths(argc, argv);

    if (!Logger::initializeDefault(state.paths.logPath, true))
    {
        std::cerr << "Failed to initialize runtime logger at: " << state.paths.logPath << std::endl;
        return 1;
    }

    if (!runParser(state))
    {
        return 1;
    }

    if (!runValidation(state))
    {
        return 1;
    }

    printSummary(state);

    if (!runExporter(state))
    {
        return 1;
    }

    LOG_INFO(LogStage::Summary, "Run summary: parsed=" + std::to_string(state.users.size()) + ", skipped=" + std::to_string(state.rejectedRecords.size()) + ", valid=" + std::to_string(state.validationResult.validUsers.size()) + ", invalid=" + std::to_string(state.validationResult.invalidUsers.size()));

    return 0;
}