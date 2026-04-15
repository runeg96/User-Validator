#pragma once

#include <string>
#include <vector>

#include "user.hpp"

class IExporter
{
public:
    virtual ~IExporter() = default;

    /// @brief Exports a vector of User objects to a specified file path.
    /// @param i_users The vector of User objects to export.
    /// @param i_exportPath The file path where the exported data should be saved.
    /// @return True if the export was successful, false otherwise.
    virtual bool exportUsers(const std::vector<User>& i_users, const std::string& i_exportPath) const = 0;
};