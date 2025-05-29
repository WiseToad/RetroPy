#ifndef UTILS_H
#define UTILS_H

#include <map>

inline std::string trimRight(const std::string &str)
{
    size_t p = str.find_last_not_of(" ");
    return (p != std::string::npos) ? str.substr(0, p + 1) : "";
}

inline std::string trimLeft(const std::string &str)
{
    size_t p = str.find_first_not_of(" ");
    return (p != std::string::npos) ? str.substr(p) : "";
}

inline std::string trim(const std::string &str)
{
    return trimLeft(trimRight(str));
}

#endif // UTILS_H
