#pragma once

#include <string>
#include <vector>

namespace utils {

/// @brief Reads the contents of a file.
/// @param path The path to the file.
/// @return The contents of the file as a string.
std::string readFile(const std::string& path);

/// @brief Joins a list of platforms into a single string.
/// @param i_platforms The list of platforms to join.
/// @return A string containing the joined platforms.
std::string joinPlatforms(const std::vector<std::string>& i_platforms);

/// @brief Splits a string of platforms into a list.
/// @param i_platforms The string of platforms to split.
/// @return A vector containing the individual platforms.
std::vector<std::string> splitPlatforms(const std::string& i_platforms);

} // namespace utils