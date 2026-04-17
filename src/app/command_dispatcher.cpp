#include "app/command_dispatcher.hpp"

#include <memory>
#include <unordered_map>

#include "app/command_names.hpp"
#include "app/user_pipeline.hpp"
#include "logging/logger.hpp"

class IAppCommand
{
public:
    virtual ~IAppCommand()                                           = default;
    virtual CommandExecutionResult execute(AppState& io_state) const = 0;
};

namespace {

class ParseUsersCommand final : public IAppCommand
{
public:
    CommandExecutionResult execute(AppState& io_state) const override
    {
        const bool parseSucceeded = m_pipeline.parse(io_state);
        return { parseSucceeded, parseSucceeded };
    }

private:
    UserPipeline m_pipeline;
};

class ValidateUsersCommand final : public IAppCommand
{
public:
    CommandExecutionResult execute(AppState& io_state) const override
    {
        if (!m_pipeline.parse(io_state))
        {
            return { false, false };
        }

        const bool validationSucceeded = m_pipeline.validate(io_state);
        return { validationSucceeded, true };
    }

private:
    UserPipeline m_pipeline;
};

class ExportUsersCommand final : public IAppCommand
{
public:
    CommandExecutionResult execute(AppState& io_state) const override
    {
        if (!m_pipeline.parse(io_state))
        {
            return { false, false };
        }

        if (!m_pipeline.validate(io_state))
        {
            return { false, true };
        }

        const bool exportSucceeded = m_pipeline.exportValidUsers(io_state);
        return { exportSucceeded, true };
    }

private:
    UserPipeline m_pipeline;
};

} // namespace

AppCommandDispatcher::AppCommandDispatcher()
{
    m_commands.emplace(app_commands::c_parseUsersCommandName, std::make_unique<ParseUsersCommand>());
    m_commands.emplace(app_commands::c_validateUsersCommandName, std::make_unique<ValidateUsersCommand>());
    m_commands.emplace(app_commands::c_exportUsersCommandName, std::make_unique<ExportUsersCommand>());
    m_commands.emplace(app_commands::c_runPipelineCommandName, std::make_unique<ExportUsersCommand>());
}

AppCommandDispatcher::~AppCommandDispatcher() = default;

CommandExecutionResult AppCommandDispatcher::dispatch(const std::string& i_commandName, AppState& io_state) const
{
    const auto iterator = m_commands.find(i_commandName);
    if (iterator == m_commands.end())
    {
        LOG_ERROR(LogStage::Summary, "Unknown command: " + i_commandName);
        return {};
    }

    LOG_INFO(LogStage::Summary, "Executing command: " + i_commandName);
    return iterator->second->execute(io_state);
}