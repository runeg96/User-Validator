#pragma once

#include <string>
#include <vector>

#include "user.hpp"

class ExportService
{
public:
    bool exportUsers(const std::vector<User>& i_users, const std::string& i_exportPath) const;
};