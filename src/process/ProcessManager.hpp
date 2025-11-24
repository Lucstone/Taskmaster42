#pragma once

#include "Process.hpp"
#include "../config/ProgramConfig.hpp"
#include <map>
#include <vector>
#include <string>

// Result enums for command operations
enum StartResult {
    START_SUCCESS = 0,
    START_ALREADY_STARTED = 1,
    START_NO_SUCH_PROCESS = 2,
    START_SPAWN_ERROR = 3,
    START_NO_SUCH_FILE = 4
};

enum StopResult {
    STOP_SUCCESS = 0,
    STOP_NOT_RUNNING = 1,
    STOP_NO_SUCH_PROCESS = 2
};

enum RestartResult {
    RESTART_SUCCESS = 0,
    RESTART_NOT_RUNNING_STARTED = 1,
    RESTART_NO_SUCH_PROCESS = 2,
    RESTART_SPAWN_ERROR = 3,
    RESTART_NO_SUCH_FILE = 4
};

// Process status structure for display
struct ProcessStatus {
    bool exists;
    std::string name;
    std::string state_str;
    std::string info;
    
    ProcessStatus() : exists(false), name(""), state_str(""), info("") {}
};

class ProcessManager {
private:
    // Map: program_name -> vector of Process instances
    std::map<std::string, std::vector<Process*> > _processes;
    std::map<std::string, ProgramConfig> _current_config;
    
    void createProcesses(const ProgramConfig& config);
    void removeProcesses(const std::string& name);
    void stopAllProcesses();
    bool configChanged(const ProgramConfig& old_cfg, const ProgramConfig& new_cfg) const;

public:
    // Default constructor
    ProcessManager();
    
    // Copy constructor (deleted - not copyable due to process ownership)
    ProcessManager(const ProcessManager& other);
    
    // Copy assignment operator (deleted)
    ProcessManager& operator=(const ProcessManager& other);
    
    // Destructor
    ~ProcessManager();
    
    // Configuration management
    void loadConfig(const std::map<std::string, ProgramConfig>& configs);
    void reloadConfig(const std::map<std::string, ProgramConfig>& new_configs);
    
    // Process control
    StartResult startProgram(const std::string& name);
    StopResult stopProgram(const std::string& name);
    RestartResult restartProgram(const std::string& name);
    
    // Start all programs marked with autostart
    void startAutostart();
    
    // Called when SIGCHLD received - reaps zombies and updates state
    void handleSigchld();
    
    // Periodic maintenance - check process states, handle restarts
    void update();
    
    // Status reporting
    std::vector<std::string> getStatusReport() const;
    std::vector<std::string> getProgramNames() const;
    ProcessStatus getProcessStatus(const std::string& name) const;
    std::vector<ProcessStatus> getAllStatus() const;
    std::string formatProcessInfo(const Process* proc) const;
    // Shutdown
    void shutdown();
};