#include "Config.h"
#include "../lib/json.hpp"
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>

using json = nlohmann::json;

ConfigData Config::load(const std::string& path) {
    ConfigData data;
    std::ifstream f(path);
    if (!f.is_open()) return data;

    try {
        json j = json::parse(f);
        if (j.contains("username")) data.username = j["username"];
        if (j.contains("password")) data.password = j["password"];
    } catch (...) {}
    return data;
}

void Config::save(const std::string& path, const ConfigData& data) {
    json j;
    j["username"] = data.username;
    j["password"] = data.password;

    std::ofstream f(path);
    f << j.dump(2);

    // Set permissions to 0600
    chmod(path.c_str(), S_IRUSR | S_IWUSR);
}

std::string Config::defaultPath() {
    const char* home = getenv("HOME");
    if (!home) home = "/root";
    return std::string(home) + "/.auth-thu";
}
