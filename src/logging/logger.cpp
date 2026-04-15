#include "logging/logger.hpp"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <system_error>
#include <utility>

#include "logging/logEntry.hpp"

namespace {

Logger& getDefaultLoggerStorage()
{
    static Logger logger;
    return logger;
}

std::string getTimestamp()
{
    const auto now            = std::chrono::system_clock::now();
    const std::time_t nowTime = std::chrono::system_clock::to_time_t(now);

    std::tm localTime{};
#if defined(_WIN32)
    localtime_s(&localTime, &nowTime);
#else
    localtime_r(&nowTime, &localTime);
#endif

    std::ostringstream stream;
    stream << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return stream.str();
}

const char* toString(const LogStage i_stage)
{
    switch (i_stage)
    {
        case LogStage::Parser:
            return "parser";
        case LogStage::Validator:
            return "validator";
        case LogStage::Export:
            return "export";
        case LogStage::Summary:
            return "summary";
        case LogStage::Schema:
            return "schema";
    }

    return "unknown";
}

const char* toString(const Severity i_severity)
{
    switch (i_severity)
    {
        case Severity::Info:
            return "info";
        case Severity::Error:
            return "error";
    }

    return "unknown";
}

} // namespace

Logger& Logger::defaultLogger()
{
    return getDefaultLoggerStorage();
}

bool Logger::initializeDefault(const std::filesystem::path& i_logPath, bool i_mirrorToConsole)
{
    Logger& logger = defaultLogger();
    logger.setMirrorToConsole(i_mirrorToConsole);
    return logger.initialize(i_logPath);
}

bool Logger::info(LogStage i_stage, const std::string& i_message, const char* i_functionName)
{
    return logDefault(LogEntry{ i_stage, Severity::Info, i_functionName, i_message });
}

bool Logger::error(LogStage i_stage, const std::string& i_message, const char* i_functionName)
{
    return logDefault(LogEntry{ i_stage, Severity::Error, i_functionName, i_message });
}

bool Logger::logDefault(const LogEntry& i_entry)
{
    return defaultLogger().log(i_entry);
}

Logger::Logger(std::filesystem::path i_logPath, bool i_mirrorToConsole)
    : m_logPath(std::move(i_logPath))
    , m_mirrorToConsole(i_mirrorToConsole)
{
}

bool Logger::initialize(const std::filesystem::path& i_logPath)
{
    m_logPath = i_logPath;

    if (m_logPath.empty())
    {
        return false;
    }

    const std::filesystem::path parentPath = m_logPath.parent_path();
    if (!parentPath.empty())
    {
        std::error_code errorCode;
        std::filesystem::create_directories(parentPath, errorCode);

        if (errorCode)
        {
            std::cerr << "Failed to create log directory: " << parentPath << std::endl;
            return false;
        }
    }

    std::ofstream output(m_logPath, std::ios::out | std::ios::trunc);
    if (!output.is_open())
    {
        std::cerr << "Failed to open log file: " << m_logPath << std::endl;
        return false;
    }

    output << "=== json_parser run started at " << getTimestamp() << " ===\n";
    return true;
}

bool Logger::log(const LogEntry& i_entry) const
{
    if (m_logPath.empty())
    {
        return false;
    }

    const std::string formattedEntry = formatEntry(i_entry);

    std::ofstream output(m_logPath, std::ios::out | std::ios::app);
    if (!output.is_open())
    {
        return false;
    }

    output << formattedEntry << '\n';

    if (m_mirrorToConsole)
    {
        std::cerr << formattedEntry << std::endl;
    }

    return true;
}

const std::filesystem::path& Logger::logPath() const
{
    return m_logPath;
}

void Logger::setMirrorToConsole(bool i_mirrorToConsole)
{
    m_mirrorToConsole = i_mirrorToConsole;
}

std::string Logger::formatEntry(const LogEntry& i_entry) const
{
    std::ostringstream stream;
    stream << '[' << getTimestamp() << "] [" << toString(i_entry.severity) << "] [" << toString(i_entry.stage) << ']';

    if (!i_entry.functionName.empty())
    {
        stream << " [" << i_entry.functionName << ']';
    }

    stream << ' ' << i_entry.message;

    return stream.str();
}