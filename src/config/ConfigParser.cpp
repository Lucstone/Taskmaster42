#include "ConfigParser.hpp"
#include "../logger/Logger.hpp"
#include "../utils/Utils.hpp"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include <sstream>
#include <stdexcept>

ConfigParser::ConfigParser() : _config_file("") {}

ConfigParser::ConfigParser(const std::string &config_file) 
    : _config_file(config_file) {}

ConfigParser::ConfigParser(const ConfigParser &other) 
    : _config_file(other._config_file) {}

ConfigParser &ConfigParser::operator=(const ConfigParser &other) {
    if (this != &other) {
        _config_file = other._config_file;
    }
    return *this;
}

ConfigParser::~ConfigParser() {}

std::map<std::string, ProgramConfig> ConfigParser::parse(const std::string &filename) {
    std::map<std::string, ProgramConfig>    configs;
    
    LOG_INFO("Parsing configuration file: " + filename);

    try {
        if (!Utils::fileExists(filename)) {
            throw std::runtime_error("Config file not found: " + filename);
        }

        YAML::Node  config = YAML::LoadFile(filename);

        if (!config["programs"]) {
            throw std::runtime_error("No 'programs' section in config file");
        }

        YAML::Node programs = config["programs"];

        for (YAML::const_iterator it = programs.begin(); it != programs.end(); ++it) {
            std::string program_name = it->first.as<std::string>();
            YAML::Node  program_node = it->second;

            LOG_DEBUG("Parsing program: " + program_name);

            ProgramConfig prog_config(program_name);
            if (!program_node["cmd"]) {
                throw std::runtime_error("Program '" + program_name + "' missing required field 'cmd'");
            }
            prog_config.setCmd(program_node["cmd"].as<std::string>());

            if (program_node["numprocs"]) {
                prog_config.setNumprocs(program_node["numprocs"].as<int>());
            } else {
                prog_config.setNumprocs(1);
            }

            if (program_node["umask"]) {
                std::string umask_str = program_node["umask"].as<std::string>();
                int         umask_val;

                std::istringstream iss(umask_str);
                iss >> std::oct >> umask_val;

                prog_config.setUmask(umask_val);
            } else {
                prog_config.setUmask(022);
            }

            if (program_node["workingdir"]) {
                prog_config.setWorkingdir(program_node["workingdir"].as<std::string>());
            } else {
                prog_config.setWorkingdir("/tmp");
            }

            if (program_node["autostart"]) {
                prog_config.setAutostart(program_node["autostart"].as<bool>());
            } else {
                prog_config.setAutostart(true);
            }

            if (program_node["autorestart"]) {
                std::string autorestart = program_node["autorestart"].as<std::string>();
                if (autorestart != "always" && autorestart != "never" && autorestart != "unexpected") {
                    throw std::runtime_error("Program '" + program_name + 
                        "': autorestart must be 'always', 'never', or 'unexpected'");
                }
                prog_config.setAutorestart(autorestart);
            } else {
                prog_config.setAutorestart("unexpected");
            }

            if (program_node["exitcodes"]) {
                std::vector<int> exitcodes;

                if (program_node["exitcodes"].IsSequence()) {
                    for (size_t i = 0; i < program_node["exitcodes"].size(); ++i) {
                        exitcodes.push_back(program_node["exitcodes"][i].as<int>());
                    }
                } else {
                    exitcodes.push_back(program_node["exitcodes"].as<int>());
                }

                prog_config.setExitcodes(exitcodes);
            } else {
                std::vector<int> default_codes;
                default_codes.push_back(0);
                prog_config.setExitcodes(default_codes);
            }

            if (program_node["startretries"]) {
                prog_config.setStartretries(program_node["startretries"].as<int>());
            } else {
                prog_config.setStartretries(3);
            }

            if (program_node["starttime"]) {
                prog_config.setStarttime(program_node["starttime"].as<int>());
            } else {
                prog_config.setStarttime(1);
            }

            if (program_node["stopsignal"]) {
                prog_config.setStopsignal(program_node["stopsignal"].as<std::string>());
            } else {
                prog_config.setStopsignal("TERM");
            }

            if (program_node["stoptime"]) {
                prog_config.setStoptime(program_node["stoptime"].as<int>());
            } else {
                prog_config.setStoptime(10);
            }

            if (program_node["stdout"]) {
                prog_config.setStdoutFile(program_node["stdout"].as<std::string>());
            } else {
                prog_config.setStdoutFile("");
            }

            if (program_node["stderr"]) {
                prog_config.setStderrFile(program_node["stderr"].as<std::string>());
            } else {
                prog_config.setStderrFile("");
            }

            if (program_node["env"]) {
                std::map<std::string, std::string>  env_map;
                YAML::Node                          env_node = program_node["env"];

                for (YAML::const_iterator env_it = env_node.begin(); 
                     env_it != env_node.end(); ++env_it) {
                    std::string key = env_it->first.as<std::string>();
                    std::string value = env_it->second.as<std::string>();
                    env_map[key] = value;
                }

                prog_config.setEnv(env_map);
            }

            validateProgramConfig(prog_config);

            configs[program_name] = prog_config;

            LOG_INFO("Program configured: " + program_name);
        }

        LOG_INFO("Successfully parsed " + std::to_string(configs.size()) + " programs");

    } catch (const YAML::Exception &e) {
        LOG_ERROR("YAML parsing error: " + std::string(e.what()));
        throw std::runtime_error("Failed to parse config file: " + std::string(e.what()));
    } catch (const std::exception &e) {
        LOG_ERROR("Config parsing error: " + std::string(e.what()));
        throw;
    }

    return configs;
}

void ConfigParser::validateProgramConfig(const ProgramConfig &config) const {
    if (config.getNumprocs() < 1) {
        throw std::runtime_error("Program '" + config.getName() + 
            "': numprocs must be >= 1");
    }

    if (config.getNumprocs() > 100) {
        throw std::runtime_error("Program '" + config.getName() + 
            "': numprocs must be <= 100 (sanity check)");
    }
    
    if (config.getStartretries() < 0) {
        throw std::runtime_error("Program '" + config.getName() + 
            "': startretries must be >= 0");
    }

    if (config.getStarttime() < 0) {
        throw std::runtime_error("Program '" + config.getName() + 
            "': starttime must be >= 0");
    }

    if (config.getStoptime() < 0) {
        throw std::runtime_error("Program '" + config.getName() + 
            "': stoptime must be >= 0");
    }

    if (config.getUmask() < 0 || config.getUmask() > 0777) {
        throw std::runtime_error("Program '" + config.getName() + 
            "': umask must be between 0 and 0777");
    }

    int sig = config.getStopSignalNumber();
    if (sig < 0) {
        throw std::runtime_error("Program '" + config.getName() + 
            "': invalid stopsignal '" + config.getStopsignal() + "'");
    }
}

bool ConfigParser::validate(const std::map<std::string, ProgramConfig> &configs) const {
    if (configs.empty()) {
        LOG_WARNING("Configuration has no programs defined");
        return false;
    }

    return true;
}
