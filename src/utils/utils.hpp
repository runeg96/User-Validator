#pragma once

#include <string>
#include <vector>

namespace utils {

std::string readFile(const std::string& path);
std::string joinPlatforms(const std::vector<std::string>& i_platforms);
std::vector<std::string> splitPlatforms(const std::string& i_platforms);

} // namespace utils