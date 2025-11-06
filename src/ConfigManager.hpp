#pragma once
#include <string>
#include <map>
#include "ProcessInfo.hpp"
#include "ConfigParser.hpp"

class ConfigManager {
private:
    std::map<std::string, ConfigParser> _configs;

public:
    const std::map<std::string, ConfigParser>& getConfigs() const { return _configs; }

    void loadConfig(const std::string& path) {
        _configs = ConfigParser::loadAll(path);
    }

    void reloadConfig(const std::string& path) {
        _configs = ConfigParser::loadAll(path);
    }
};
