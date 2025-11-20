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
    ConfigParser(const std::string& config_file);
    ConfigParser(const ConfigParser& other);
    ConfigParser& operator=(const ConfigParser& other);
    ~ConfigParser();

    std::map<std::string, ProgramConfig> parse(const std::string& filename);

    bool validate(const std::map<std::string, ProgramConfig>& configs) const;
    void validateProgramConfig(const ProgramConfig& config) const;

    void printConfigs(const std::map<std::string, ProgramConfig>& configs) {
        std::cout << "=== Parsed Programs ===\n";
        for (const auto& [name, cfg] : configs) {
            std::cout << "Program: " << name << "\n";
            std::cout << "  cmd: " << cfg.getCmd() << "\n";
            std::cout << "  numprocs: " << cfg.getNumprocs() << "\n";
            std::cout << "  umask: " << std::oct << cfg.getUmask() << std::dec << "\n"; // affiche en octal
            std::cout << "  workingdir: " << cfg.getWorkingdir() << "\n";
            std::cout << "  autostart: " << (cfg.getAutostart() ? "true" : "false") << "\n";
            std::cout << "  autorestart: " << cfg.getAutorestart() << "\n";
            std::cout << "  exitcodes: ";
            for (int code : cfg.getExitcodes()) std::cout << code << " ";
            std::cout << "\n";
            std::cout << "  startretries: " << cfg.getStartretries() << "\n";
            std::cout << "  starttime: " << cfg.getStarttime() << "\n";
            std::cout << "  stoptime: " << cfg.getStoptime() << "\n";
            std::cout << "  stopsignal: " << cfg.getStopsignal() << "\n";
            std::cout << "  stdout_file: " << cfg.getStdoutFile() << "\n";
            std::cout << "  stderr_file: " << cfg.getStderrFile() << "\n";

            const auto& env = cfg.getEnv();
            if (!env.empty()) {
                std::cout << "  environment:\n";
                for (const auto& [key, val] : env) {
                    std::cout << "    " << key << "=" << val << "\n";
                }
            }

            std::cout << "----------------------\n";
        }
        std::cout << "Total programs parsed: " << configs.size() << "\n";
    }
};