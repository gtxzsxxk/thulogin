#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct ConfigData {
    std::string username;
    std::string password;
};

class Config {
public:
    static ConfigData load(const std::string& path);
    static void save(const std::string& path, const ConfigData& data);
    static std::string defaultPath();
};

#endif
