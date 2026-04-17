#pragma once

#include <string>

namespace app_commands {

inline constexpr const char* c_parseUsersCommandName    = "parse_users";
inline constexpr const char* c_validateUsersCommandName = "validate_users";
inline constexpr const char* c_exportUsersCommandName   = "export_users";
inline constexpr const char* c_runPipelineCommandName   = "run_pipeline";

inline bool isKnownCommandName(const std::string& i_commandName)
{
    return i_commandName == c_parseUsersCommandName || i_commandName == c_validateUsersCommandName || i_commandName == c_exportUsersCommandName || i_commandName == c_runPipelineCommandName;
}

} // namespace app_commands