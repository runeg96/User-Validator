#include "utils.hpp"

#include <fstream>
#include <iostream>
#include <sstream>

namespace utils {

std::string readFile(const std::string& path)
{
    std::ifstream input(path);
    if (!input.is_open())
    {
        std::cerr << "Could not open file: " << path << std::endl;
        return {};
    }

    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

std::string joinPlatforms(const std::vector<std::string>& i_platforms)
{
    std::string result;

    for (size_t i = 0; i < i_platforms.size(); ++i)
    {
        if (i > 0)
        {
            result += ", ";
        }

        result += i_platforms[i];
    }

    return result;
}

std::vector<std::string> splitPlatforms(const std::string& i_platforms)
{
    std::vector<std::string> result;
    size_t start = 0;

    while (start < i_platforms.size())
    {
        size_t end = i_platforms.find(',', start);
        if (end == std::string::npos)
        {
            end = i_platforms.size();
        }

        std::string item = i_platforms.substr(start, end - start);

        const size_t left  = item.find_first_not_of(' ');
        const size_t right = item.find_last_not_of(' ');

        if (left != std::string::npos)
        {
            result.push_back(item.substr(left, right - left + 1));
        }

        start = end + 1;
    }

    return result;
}

} // namespace utils