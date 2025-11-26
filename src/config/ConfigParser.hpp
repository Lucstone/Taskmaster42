#pragma once

#include "ProgramConfig.hpp"
#include <iostream>
#include <map>
#include <vector>
#include <string>

class ConfigParser {
private:
    std::string _config_file;

public:
    ConfigParser();
    ConfigParser(const std::string &config_file);
    ConfigParser(const ConfigParser &other);
    ConfigParser &operator=(const ConfigParser &other);
    ~ConfigParser();

    std::map<std::string, ProgramConfig>    parse(const std::string &filename);

    bool                                    validate(const std::map<std::string, ProgramConfig> &configs) const;
    void                                    validateProgramConfig(const ProgramConfig &config) const;
};
