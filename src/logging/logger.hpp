#pragma once

#include <filesystem>
#include <string>

#include "logging/logEntry.hpp"

class Logger
{
public:
    static Logger& defaultLogger();
    static bool initializeDefault(const std::filesystem::path& i_logPath, bool i_mirrorToConsole = false);
    static bool info(LogStage i_stage, const std::string& i_message, const char* i_functionName);
    static bool error(LogStage i_stage, const std::string& i_message, const char* i_functionName);
    static bool logDefault(const LogEntry& i_entry);

    Logger() = default;
    explicit Logger(std::filesystem::path i_logPath, bool i_mirrorToConsole = false);

    bool initialize(const std::filesystem::path& i_logPath);
    bool log(const LogEntry& i_entry) const;

    const std::filesystem::path& logPath() const;
    void setMirrorToConsole(bool i_mirrorToConsole);

private:
    std::string formatEntry(const LogEntry& i_entry) const;

    std::filesystem::path m_logPath;
    bool m_mirrorToConsole = false;
};

#define LOG_INFO(stage, message) Logger::info(stage, message, __func__)
#define LOG_ERROR(stage, message) Logger::error(stage, message, __func__)