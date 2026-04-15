#include <chrono>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "exporter/export_service.hpp"
#include "logging/logEntry.hpp"
#include "logging/logger.hpp"
#include "parser/user_parser.hpp"
#include "user.hpp"
#include "validator/record_error.hpp"
#include "validator/user_validator.hpp"

namespace {

struct AppPaths
{
    std::filesystem::path inputJsonPath;
    std::filesystem::path schemaPath;
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
    std::vector<RecordError> parseErrors;
    ValidationResult validationResult;
    Logger logger;
    long long parseDurationUs      = 0;
    long long validationDurationUs = 0;
};

void logMessage(const Logger& i_logger,
                const std::string& i_stage,
                const std::string& i_severity,
                const std::string& i_message,
                std::optional<size_t> i_userIndex = std::nullopt,
                std::optional<std::string> i_userName = std::nullopt)
{
    i_logger.log(LogEntry{ i_stage, i_severity, std::move(i_userIndex), std::move(i_userName), i_message });
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
    paths.schemaPath     = i_argc > 2 ? std::filesystem::path(i_argv[2]) : exeDir / "users.schema.json";
    paths.jsonExportPath = exeDir / "users_export.json";
    paths.xmlExportPath  = exeDir / "users_export.xml";
    paths.logPath        = exeDir / "logs" / "parser.log";

    return paths;
}

ValidationResult validateUsers(const std::vector<User>& i_users,
                               const UserValidator& i_validator,
                               const Logger& i_logger)
{
    ValidationResult result;

    for (size_t userIndex = 0; userIndex < i_users.size(); ++userIndex)
    {
        const auto& user = i_users[userIndex];
        const std::vector<ValidationError> errors = i_validator.validate(user);

        if (errors.empty())
        {
            result.validUsers.push_back(user);
        }
        else
        {
            result.invalidUsers.push_back({ user, errors });

            for (const auto& error : errors)
            {
                logMessage(i_logger,
                           "validator",
                           "error",
                           error.field + ": " + error.message,
                           userIndex,
                           user.name);
            }
        }
    }

    return result;
}

void printSummary(const AppState& i_state)
{
    std::cout << "Parsed " << i_state.users.size() << " users\n";
    std::cout << "Skipped during parser/schema: " << i_state.parseErrors.size() << "\n";
    std::cout << "Valid users: " << i_state.validationResult.validUsers.size() << "\n";
    std::cout << "Invalid users: " << i_state.validationResult.invalidUsers.size() << "\n";
    std::cout << "Parsing took " << i_state.parseDurationUs << " us\n";
    std::cout << "Validation took " << i_state.validationDurationUs << " us\n";
}

bool runParser(AppState& io_state)
{
    UserParser parser;
    if (!parser.init(io_state.paths.schemaPath.string()))
    {
        logMessage(io_state.logger,
                   "parser",
                   "error",
                   "Failed to initialize parser with schema: " + io_state.paths.schemaPath.string());
        std::cerr << "Failed to initialize parser with schema: "
                  << io_state.paths.schemaPath << std::endl;
        return false;
    }

    logMessage(io_state.logger,
               "parser",
               "info",
               "Parsing users from " + io_state.paths.inputJsonPath.string());

    const auto parseStartTime = std::chrono::steady_clock::now();
    const bool parsed         = parser.parseUsersFromFile(io_state.paths.inputJsonPath.string(),
                                                  io_state.users,
                                                  io_state.parseErrors,
                                                  &io_state.logger);
    const auto parseEndTime   = std::chrono::steady_clock::now();

    if (!parsed)
    {
        std::cerr << "Failed to parse users from: "
                  << io_state.paths.inputJsonPath << std::endl;
        return false;
    }

    io_state.parseDurationUs = std::chrono::duration_cast<std::chrono::microseconds>(parseEndTime - parseStartTime).count();

    logMessage(io_state.logger,
               "parser",
               "info",
               "Parsed " + std::to_string(io_state.users.size()) + " users and skipped "
                   + std::to_string(io_state.parseErrors.size()) + " records during parser/schema stage.");

    return true;
}

bool runValidation(AppState& io_state)
{
    UserValidator validator;

    logMessage(io_state.logger,
               "validator",
               "info",
               "Validating " + std::to_string(io_state.users.size()) + " parsed users.");

    const auto validationStartTime = std::chrono::steady_clock::now();
    io_state.validationResult      = validateUsers(io_state.users, validator, io_state.logger);
    const auto validationEndTime   = std::chrono::steady_clock::now();

    io_state.validationDurationUs = std::chrono::duration_cast<std::chrono::microseconds>(validationEndTime - validationStartTime).count();

    logMessage(io_state.logger,
               "validator",
               "info",
               "Validation produced " + std::to_string(io_state.validationResult.validUsers.size())
                   + " valid users and " + std::to_string(io_state.validationResult.invalidUsers.size())
                   + " invalid users.");

    return true;
}

bool runExporter(const AppState& i_state)
{
    ExportService exportService;
    if (exportService.exportUsers(i_state.validationResult.validUsers, i_state.paths.jsonExportPath.string()))
    {
        logMessage(i_state.logger,
                   "export",
                   "info",
                   "Successfully exported valid users to: " + i_state.paths.jsonExportPath.string());
        std::cout << "Successfully exported valid users to: " << i_state.paths.jsonExportPath << std::endl;
    }
    else
    {
        logMessage(i_state.logger,
                   "export",
                   "error",
                   "Failed to export valid users to: " + i_state.paths.jsonExportPath.string());
        std::cerr << "Failed to export valid users to: " << i_state.paths.jsonExportPath << std::endl;
        return false;
    }

    if (exportService.exportUsers(i_state.validationResult.validUsers, i_state.paths.xmlExportPath.string()))
    {
        logMessage(i_state.logger,
                   "export",
                   "info",
                   "Successfully exported valid users to: " + i_state.paths.xmlExportPath.string());
        std::cout << "Successfully exported valid users to: " << i_state.paths.xmlExportPath << std::endl;
    }
    else
    {
        logMessage(i_state.logger,
                   "export",
                   "error",
                   "Failed to export valid users to: " + i_state.paths.xmlExportPath.string());
        std::cerr << "Failed to export valid users to: " << i_state.paths.xmlExportPath << std::endl;
        return false;
    }

    return true;
}

} // namespace

int main(int argc, char* argv[])
{
    AppState state;
    state.paths = resolvePaths(argc, argv);
    state.logger.setMirrorToConsole(true);

    if (!state.logger.initialize(state.paths.logPath))
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

    logMessage(state.logger,
               "summary",
               "info",
               "Run summary: parsed=" + std::to_string(state.users.size())
                   + ", skipped=" + std::to_string(state.parseErrors.size())
                   + ", valid=" + std::to_string(state.validationResult.validUsers.size())
                   + ", invalid=" + std::to_string(state.validationResult.invalidUsers.size()));

    if (!runExporter(state))
    {
        return 1;
    }

    return 0;
}