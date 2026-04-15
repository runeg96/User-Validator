#pragma once

#include "exporter/i_exporter.hpp"

class JsonExporter : public IExporter
{
public:
    /// @brief Exports an array of User objects to a specified file path in JSON format
    /// @param i_users The vector of User objects to export.
    /// @param i_exportPath The file path where the exported data should be saved.
    /// @return True if the export was successful, false otherwise.
    bool exportUsers(const std::vector<User>& i_users, const std::string& i_exportPath) const override;
};