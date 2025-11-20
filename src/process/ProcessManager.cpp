#include "ProcessManager.hpp"
#include "../logger/Logger.hpp"
#include "../utils/Utils.hpp"
#include <iostream>
#include <sys/wait.h>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <set>
#include <unistd.h>

ProcessManager::ProcessManager()
    : _processes(),
      _current_config() {
}

ProcessManager::ProcessManager(const ProcessManager& other) {
    (void)other;
    // Not copyable
}

ProcessManager& ProcessManager::operator=(const ProcessManager& other) {
    (void)other;
    // Not copyable
    return *this;
}

ProcessManager::~ProcessManager() {
    // Clean up all processes
    for (std::map<std::string, std::vector<Process*> >::iterator it = _processes.begin();
         it != _processes.end(); ++it) {
        for (size_t i = 0; i < it->second.size(); ++i) {
            delete it->second[i];
        }
    }
    _processes.clear();
}

void ProcessManager::loadConfig(const std::map<std::string, ProgramConfig>& configs) {
    LOG_INFO("Loading configuration");
    
    _current_config = configs;
    
    // Create processes for each program
    for (std::map<std::string, ProgramConfig>::const_iterator it = configs.begin();
         it != configs.end(); ++it) {
        createProcesses(it->second);
    }
    
    LOG_INFO("Configuration loaded with " + std::to_string(configs.size()) + " programs");
}

void ProcessManager::createProcesses(const ProgramConfig& config) {
    LOG_INFO("Creating " + std::to_string(config.getNumprocs()) + 
             " instances of " + config.getName());
    
    std::vector<Process*> instances;
    
    for (int i = 0; i < config.getNumprocs(); ++i) {
        Process* proc = new Process(config, i);
        instances.push_back(proc);
    }
    
    _processes[config.getName()] = instances;
}

void ProcessManager::removeProcesses(const std::string& name) {
    LOG_INFO("Removing program: " + name);
    
    std::map<std::string, std::vector<Process*> >::iterator it = _processes.find(name);
    if (it != _processes.end()) {
        // Stop and delete all instances
        for (size_t i = 0; i < it->second.size(); ++i) {
            it->second[i]->stop();
            delete it->second[i];
        }
        _processes.erase(it);
    }
}

void ProcessManager::startAutostart() {
    LOG_INFO("Starting autostart programs");
    
    for (std::map<std::string, ProgramConfig>::const_iterator it = _current_config.begin();
         it != _current_config.end(); ++it) {
        if (it->second.getAutostart()) {
            startProgram(it->first);
        }
    }
}

StartResult ProcessManager::startProgram(const std::string& name) {
    LOG_INFO("Starting program: " + name);
    
    // Find program
    std::map<std::string, std::vector<Process*> >::iterator it = _processes.find(name);
    if (it == _processes.end()) {
        return START_NO_SUCH_PROCESS;
    }
    
    bool any_started = false;
    bool already_running = false;
    bool spawn_error = false;
    bool no_such_file = false;
    
    // Start all instances
    for (size_t i = 0; i < it->second.size(); ++i) {
        Process* proc = it->second[i];
        
        if (proc->isRunning()) {
            already_running = true;
            continue;
        }
        
        if (proc->start()) {
            any_started = true;
        } else {
            // Check state to determine error type
            if (proc->getState() == ProcessState::BACKOFF) {
                no_such_file = true;
            } else {
                spawn_error = true;
            }
        }
    }
    
    // Determine result
    if (already_running && !any_started) {
        return START_ALREADY_STARTED;
    }
    if (no_such_file) {
        return START_NO_SUCH_FILE;
    }
    if (spawn_error) {
        return START_SPAWN_ERROR;
    }
    
    return START_SUCCESS;
}

StopResult ProcessManager::stopProgram(const std::string& name) {
    LOG_INFO("Stopping program: " + name);
    
    // Find program
    std::map<std::string, std::vector<Process*> >::iterator it = _processes.find(name);
    if (it == _processes.end()) {
        return STOP_NO_SUCH_PROCESS;
    }
    
    bool any_running = false;
    
    // Stop all instances
    for (size_t i = 0; i < it->second.size(); ++i) {
        Process* proc = it->second[i];
        
        if (proc->isRunning()) {
            any_running = true;
            proc->stop();
        }
    }
    
    if (!any_running) {
        return STOP_NOT_RUNNING;
    }
    
    return STOP_SUCCESS;
}

RestartResult ProcessManager::restartProgram(const std::string& name) {
    LOG_INFO("Restarting program: " + name);
    
    // Find program
    std::map<std::string, std::vector<Process*> >::iterator it = _processes.find(name);
    if (it == _processes.end()) {
        return RESTART_NO_SUCH_PROCESS;
    }
    
    bool was_running = false;
    
    // Restart all instances
    for (size_t i = 0; i < it->second.size(); ++i) {
        Process* proc = it->second[i];
        
        if (proc->isRunning()) {
            was_running = true;
            std::cout << proc->getProgramName() << ": stopped\n";
            proc->stop();
            // Small delay for graceful stop
            sleep(1);
        }
        
        if (proc->start()) {
            std::cout << proc->getProgramName() << ": started\n";
        } else {
            // Check error type
            if (proc->getState() == ProcessState::BACKOFF) {
                return RESTART_NO_SUCH_FILE;
            } else {
                return RESTART_SPAWN_ERROR;
            }
        }
    }
    
    if (!was_running) {
        return RESTART_NOT_RUNNING_STARTED;
    }
    
    return RESTART_SUCCESS;
}

void ProcessManager::handleSigchld() {
    int status;
    pid_t pid;
    
    // Reap all terminated children (non-blocking)
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        LOG_DEBUG("Reaped process PID: " + std::to_string(pid));
        
        // Find which process this PID belongs to
        for (std::map<std::string, std::vector<Process*> >::iterator it = _processes.begin();
             it != _processes.end(); ++it) {
            for (size_t i = 0; i < it->second.size(); ++i) {
                Process* proc = it->second[i];
                if (proc->getPid() == pid) {
                    proc->handleProcessExit(status);
                    break;
                }
            }
        }
    }
}

void ProcessManager::update() {
    // Update all processes
    for (std::map<std::string, std::vector<Process*> >::iterator it = _processes.begin();
         it != _processes.end(); ++it) {
        for (size_t i = 0; i < it->second.size(); ++i) {
            it->second[i]->updateState();
        }
    }
}

std::vector<std::string> ProcessManager::getProgramNames() const {
    std::vector<std::string> names;
    
    for (std::map<std::string, std::vector<Process*> >::const_iterator it = _processes.begin();
         it != _processes.end(); ++it) {
        names.push_back(it->first);
    }
    
    return names;
}

ProcessStatus ProcessManager::getProcessStatus(const std::string& name) const {
    ProcessStatus status;
    
    // Find program (could be instance name like "program_0" or just "program")
    for (std::map<std::string, std::vector<Process*> >::const_iterator it = _processes.begin();
         it != _processes.end(); ++it) {
        for (size_t i = 0; i < it->second.size(); ++i) {
            Process* proc = it->second[i];
            
            if (proc->getName() == name || proc->getProgramName() == name) {
                status.exists = true;
                status.name = proc->getName();
                status.state_str = processStateToString(proc->getState());
                status.info = formatProcessInfo(proc);
                return status;
            }
        }
    }
    
    status.exists = false;
    status.name = name;
    return status;
}

std::vector<ProcessStatus> ProcessManager::getAllStatus() const {
    std::vector<ProcessStatus> statuses;
    
    for (std::map<std::string, std::vector<Process*> >::const_iterator it = _processes.begin();
         it != _processes.end(); ++it) {
        for (size_t i = 0; i < it->second.size(); ++i) {
            Process* proc = it->second[i];
            
            ProcessStatus status;
            status.exists = true;
            status.name = proc->getName();
            status.state_str = processStateToString(proc->getState());
            status.info = formatProcessInfo(proc);
            
            statuses.push_back(status);
        }
    }
    
    return statuses;
}

std::string ProcessManager::formatProcessInfo(const Process* proc) const {
    std::ostringstream oss;
    
    switch (proc->getState()) {
        case ProcessState::STOPPED:
            if (proc->getRestartCount() == 0) {
                oss << "Not started";
            } else {
                // Format timestamp
                time_t now = time(NULL);
                struct tm* timeinfo = localtime(&now);
                char buffer[80];
                strftime(buffer, sizeof(buffer), "%b %d %I:%M %p", timeinfo);
                oss << buffer;
            }
            break;
            
        case ProcessState::STARTING:
            // No additional info
            break;
            
        case ProcessState::RUNNING:
            {
                time_t uptime = proc->getUptime();
                int hours = uptime / 3600;
                int mins = (uptime % 3600) / 60;
                int secs = uptime % 60;
                
                oss << "pid " << proc->getPid() << ", uptime "
                    << hours << ":"
                    << std::setw(2) << std::setfill('0') << mins << ":"
                    << std::setw(2) << std::setfill('0') << secs;
            }
            break;
            
        case ProcessState::BACKOFF:
        case ProcessState::FATAL:
            // Check if it's a "no such file" error
            // (This is a simplification - you might want to store error type)
            oss << "Exited too quickly (process log may have details)";
            break;
            
        case ProcessState::EXITED:
            oss << "Exited with code " << proc->getExitCode();
            break;
            
        default:
            break;
    }
    
    return oss.str();
}

void ProcessManager::reloadConfig(const std::map<std::string, ProgramConfig>& new_configs) {
    LOG_INFO("Reloading configuration");
    
    std::set<std::string> old_names, new_names;
    
    // Get all program names
    for (std::map<std::string, ProgramConfig>::const_iterator it = _current_config.begin();
         it != _current_config.end(); ++it) {
        old_names.insert(it->first);
    }
    
    for (std::map<std::string, ProgramConfig>::const_iterator it = new_configs.begin();
         it != new_configs.end(); ++it) {
        new_names.insert(it->first);
    }
    
    // 1. Remove programs that are no longer in config
    for (std::set<std::string>::const_iterator it = old_names.begin();
         it != old_names.end(); ++it) {
        if (new_names.find(*it) == new_names.end()) {
            LOG_INFO("Removing program: " + *it);
            removeProcesses(*it);
        }
    }
    
    // 2. Add new programs
    for (std::map<std::string, ProgramConfig>::const_iterator it = new_configs.begin();
         it != new_configs.end(); ++it) {
        const std::string& name = it->first;
        const ProgramConfig& new_config = it->second;
        
        if (old_names.find(name) == old_names.end()) {
            // New program
            LOG_INFO("Adding program: " + name);
            createProcesses(new_config);
            if (new_config.getAutostart()) {
                startProgram(name);
            }
        } else {
            // Existing program - check if changed
            const ProgramConfig& old_config = _current_config[name];
            
            if (configChanged(old_config, new_config)) {
                LOG_INFO("Program configuration changed: " + name);
                stopProgram(name);
                removeProcesses(name);
                createProcesses(new_config);
                if (new_config.getAutostart()) {
                    startProgram(name);
                }
            } else {
                LOG_INFO("Program unchanged: " + name);
                // Keep running!
            }
        }
    }
    
    _current_config = new_configs;
    LOG_INFO("Configuration reload complete");
}

bool ProcessManager::configChanged(const ProgramConfig& old_cfg, const ProgramConfig& new_cfg) const {
    return old_cfg != new_cfg;
}

void ProcessManager::stopAllProcesses() {
    LOG_INFO("Stopping all processes");
    
    for (std::map<std::string, std::vector<Process*> >::iterator it = _processes.begin();
         it != _processes.end(); ++it) {
        stopProgram(it->first);
    }
}

void ProcessManager::shutdown() {
    LOG_INFO("Shutting down process manager");
    stopAllProcesses();
    
    // Wait a bit for graceful shutdown
    sleep(2);
    
    // Force kill any remaining processes
    for (std::map<std::string, std::vector<Process*> >::iterator it = _processes.begin();
         it != _processes.end(); ++it) {
        for (size_t i = 0; i < it->second.size(); ++i) {
            it->second[i]->kill();
        }
    }
}

std::vector<std::string> ProcessManager::getStatusReport() const {
    std::vector<std::string> report;
    
    // Deprecated - use getAllStatus() instead
    std::vector<ProcessStatus> statuses = getAllStatus();
    
    for (size_t i = 0; i < statuses.size(); ++i) {
        report.push_back(statuses[i].name + " " + statuses[i].state_str + " " + statuses[i].info);
    }
    
    return report;
}