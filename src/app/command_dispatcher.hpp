#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "app/app_state.hpp"

class IAppCommand;

struct CommandExecutionResult
{
    bool success            = false;
    bool shouldPrintSummary = false;
};

class AppCommandDispatcher
{
public:
    AppCommandDispatcher();
    ~AppCommandDispatcher();

    CommandExecutionResult dispatch(const std::string& i_commandName, AppState& io_state) const;

private:
    std::unordered_map<std::string, std::unique_ptr<IAppCommand>> m_commands;
};