#pragma once

#include <cstddef>
#include <optional>
#include <string>

struct LogEntry
{
    std::string stage;
    std::string severity;
    std::optional<std::size_t> userIndex;
    std::optional<std::string> userName;
    std::string message;
};