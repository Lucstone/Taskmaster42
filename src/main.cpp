#include "config/ConfigParser.hpp"
#include "config/ProgramConfig.hpp"
#include "process/ProcessManager.hpp"
#include "shell/Shell.hpp"
#include "shell/CommandHandler.hpp"
#include "signal/SignalHandler.hpp"
#include "logger/Logger.hpp"
#include "utils/ArgsParser.hpp"
#include "utils/Utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>

class Taskmaster {
private:
    std::string     _config_file;
    ConfigParser    _config_parser;
    ProcessManager  _process_manager;
    CommandHandler  _command_handler;
    Shell           _shell;
    bool            _running;
    
    void loadConfiguration() {
        LOG_INFO("Loading configuration from: " + _config_file);
        
        try {
            std::map<std::string, ProgramConfig> configs = _config_parser.parse(_config_file);
            //_config_parser.printConfigs(configs);
            _process_manager.loadConfig(configs);
            LOG_INFO("Configuration loaded successfully");
        } catch (const std::exception& e) {
            LOG_ERROR(std::string("Failed to load configuration: ") + e.what());
            throw;
        }
    }

    void reloadConfiguration() {
        LOG_INFO("Reloading configuration");
        
        try {
            std::map<std::string, ProgramConfig> new_configs = _config_parser.parse(_config_file);
            _process_manager.reloadConfig(new_configs);
            LOG_INFO("Configuration reloaded successfully");
            std::cout << "taskmaster: configuration reloaded\n";
        } catch (const std::exception& e) {
            LOG_ERROR(std::string("Failed to reload configuration: ") + e.what());
            std::cerr << "taskmaster: ERROR - Failed to reload configuration\n";
        }
    }

    void shutdown() {
        LOG_INFO("Shutting down taskmaster");
        std::cout << "taskmaster: shutting down...\n";
        
        _process_manager.shutdown();
        
        std::cout << "taskmaster: stopped\n";
        LOG_INFO("Taskmaster stopped");
    }

public:
    Taskmaster(const std::string& config_file)
        : _config_file(config_file),
          _config_parser(config_file),
          _process_manager(),
          _command_handler(&_process_manager),
          _shell(&_command_handler),
          _running(true) {
    }
    
    ~Taskmaster() { }
    
    void run() {
        SignalHandler::setup();
        LOG_INFO("Signal handlers initialized");
        
        loadConfiguration();
        
        _process_manager.startAutostart();
        LOG_INFO("Autostart programs launched");
        
        std::cout << "taskmaster: started\n";
        
        _shell.setReloadCallback([this]() {
            this->reloadConfiguration();
        });

        _shell.run();
        shutdown();
    }
};

int main(int argc, char* argv[]) {
    std::string config_file = parseArguments(argc, argv);
    
    if (config_file.empty()) {
        return 1;
    }

    try {
        if (!Utils::fileExists("logs")) {
            Utils::createDirectory("logs");
        }

        Logger::getInstance("logs/taskmaster.log")->setMinLevel(LogLevel::DEBUG);
        LOG_INFO("========================================");
        LOG_INFO("Taskmaster starting");
        LOG_INFO("========================================");
        
        Taskmaster taskmaster(config_file);
        taskmaster.run();
        
        Logger::destroyInstance();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "taskmaster: FATAL ERROR - " << e.what() << "\n";
        LOG_ERROR(std::string("Fatal error: ") + e.what());
        Logger::destroyInstance();
        return 1;
    }
}
