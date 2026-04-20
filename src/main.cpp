#include <filesystem>
#include <iostream>
#include <sstream>

#include "app/app_state.hpp"
#include "app/app_summary.hpp"
#include "app/command_dispatcher.hpp"
#include "app/command_names.hpp"
#include "logging/logEntry.hpp"
#include "logging/logger.hpp"

namespace {

std::string buildHelpMessage()
{
    std::ostringstream help;
    help << "Usage: user_validator [command]\n\n";
    help << "Commands:\n";
    help << "  " << app_commands::c_parseUsersCommandName << "     Parse users from users.json\n";
    help << "  " << app_commands::c_validateUsersCommandName << "  Parse and validate users\n";
    help << "  " << app_commands::c_exportUsersCommandName << "    Parse, validate, and export valid users\n";
    help << "  " << app_commands::c_runPipelineCommandName << "    Run the full pipeline (default when no command is provided)\n\n";
    help << "Options:\n";
    help << "  -h, -help, --help, help  Show this help message\n";
    return help.str();
}

bool isHelpArgument(const std::string& i_argument)
{
    return i_argument == "-h" || i_argument == "-help" || i_argument == "--help" || i_argument == "help";
}

AppState createDefaultState(const char* i_argv0)
{
    const std::filesystem::path execDir = std::filesystem::canonical(i_argv0).parent_path();

    AppState state;
    state.inputJsonPath  = execDir / "users.json";
    state.jsonExportPath = execDir / "export" / "users_export.json";
    state.xmlExportPath  = execDir / "export" / "users_export.xml";
    state.logPath        = execDir / "logs" / "parser.log";
    return state;
}

} // namespace

int main(int argc, char* argv[])
{
    if (argc > 2)
    {
        std::cerr << "Too many arguments.\n\n"
                  << buildHelpMessage() << std::endl;
        return 1;
    }

    if (argc == 2 && isHelpArgument(argv[1]))
    {
        std::cout << buildHelpMessage() << std::endl;
        return 0;
    }

    const std::string commandName = (argc == 2) ? argv[1] : app_commands::c_runPipelineCommandName;

    if (argc == 2 && !app_commands::isKnownCommandName(commandName))
    {
        std::cerr << "Unknown command: " << commandName << "\n\n"
                  << buildHelpMessage() << std::endl;
        return 1;
    }

    AppState state = createDefaultState(argv[0]);

    if (!Logger::initializeDefault(state.logPath, true))
    {
        std::cerr << "Failed to initialize runtime logger at: " << state.logPath << std::endl;
        return 1;
    }

    AppCommandDispatcher dispatcher;
    const CommandExecutionResult result = dispatcher.dispatch(commandName, state);

    if (result.shouldPrintSummary)
    {
        printSummary(state);
    }

    if (!result.success)
    {
        return 1;
    }

    LOG_INFO(LogStage::Summary, buildSummaryLogMessage(state));

    return 0;
}