#pragma once

#include <filesystem>
#include <string>

struct LogEntry;

class Logger
{
public:
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