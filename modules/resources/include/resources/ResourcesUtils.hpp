
#ifndef RESOURCES_UTILS_HPP
#define RESOURCES_UTILS_HPP

#include <string>
#include <algorithm>
#include <cctype>

inline std::string toLower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
    return str;
}

#endif /* RESOURCES_UTILS_HPP */