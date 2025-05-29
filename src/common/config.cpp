#include "config.h"
#include "utils.h"

bool Config::load(std::istream &stream)
{
    clear();

    std::string line;
    while (std::getline(stream, line))
    {
        size_t p = line.find("#");
        if (p != std::string::npos)
            line.resize(p);

        line = trimRight(line);
        if (line.empty())
            continue;

        p = line.find("=");
        if (p == std::string::npos)
            return false;

        std::string key = trim(line.substr(0, p));
        std::string value = trim(line.substr(p + 1));

        put(key, value);
    }

    return stream.eof() || !stream.fail();
}

void Config::clear()
{
    config.clear();
}

const std::string &Config::get(const std::string &key, const std::string &defaultValue) const
{
    auto it = config.find(key);
    return it != config.end() ? it->second : defaultValue;
}

void Config::put(const std::string &key, const std::string &value) 
{
    config[key] = value;
}

bool Config::contains(const std::string &key) const
{
    return config.find(key) != config.end();
}
