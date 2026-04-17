#pragma once

#include <filesystem>
#include <vector>

#include "errors/invalid_user.hpp"
#include "errors/rejected_record.hpp"
#include "user.hpp"

struct AppState
{
    std::filesystem::path inputJsonPath;
    std::filesystem::path jsonExportPath;
    std::filesystem::path xmlExportPath;
    std::filesystem::path logPath;

    std::vector<User> users;
    std::vector<RejectedRecord> rejectedRecords;
    std::vector<User> validUsers;
    std::vector<InvalidUser> invalidUsers;

    long long parseDurationUs      = 0;
    long long validationDurationUs = 0;
};