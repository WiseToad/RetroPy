#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <istream>

class Config final
{
public:
    bool load(std::istream &stream);

    void clear();

    const std::string &get(const std::string &key, const std::string &defaultValue = "") const;
    void put(const std::string &key, const std::string &value);
    bool contains(const std::string &key) const;

private:
    std::map<std::string, std::string> config;
};

#endif // CONFIG_H
