#pragma once

#include <string>
#include <vector>

#include "user.hpp"

class ExportService
{
public:
    /// @brief Exports a list of users to the specified file path.
    /// @param i_users The list of users to export.
    /// @param i_exportPath The file path where the users will be exported.
    /// @return True if the export was successful, false otherwise.
    bool exportUsers(const std::vector<User>& i_users, const std::string& i_exportPath) const;
};