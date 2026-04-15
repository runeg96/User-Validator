#include "file_utils.hpp"

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

} // namespace utils