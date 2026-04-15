#pragma once

#include <string>

enum class LogStage
{
    Parser,
    Validator,
    Export,
    Summary,
    Schema
};

enum class Severity
{
    Info  = 0,
    Error = 1
};

struct LogEntry
{
    LogStage stage;
    Severity severity;
    std::string functionName;
    std::string message;
};