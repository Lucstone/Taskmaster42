#pragma once

#include "Process.hpp"
#include "../config/ProgramConfig.hpp"
#include <map>
#include <vector>
#include <string>

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

struct ProcessStatus {
    bool        exists;
    std::string name;
    std::string state_str;
    std::string info;

    ProcessStatus() : exists(false), name(""), state_str(""), info("") {}
};

class ProcessManager {
private:
    std::map<std::string, std::vector<Process*> >   _processes;
    std::map<std::string, ProgramConfig>            _current_config;

    void                        createProcesses(const ProgramConfig &config);
    void                        removeProcesses(const std::string &name);
    void                        stopAllProcesses();
    bool                        configChanged(const ProgramConfig &old_cfg, const ProgramConfig &new_cfg) const;

public:
    ProcessManager();
    ProcessManager(const ProcessManager &other);
    ProcessManager &operator=(const ProcessManager &other);
    ~ProcessManager();

    void                        loadConfig(const std::map<std::string, ProgramConfig> &configs);
    void                        reloadConfig(const std::map<std::string, ProgramConfig> &new_configs);

    StartResult                 startProgram(const std::string &name);
    StopResult                  stopProgram(const std::string &name);
    RestartResult               restartProgram(const std::string &name);

    void                        startAutostart();
    void                        handleSigchld();
    void                        update();

    std::vector<std::string>    getStatusReport() const;
    std::vector<std::string>    getProgramNames() const;
    ProcessStatus               getProcessStatus(const std::string &name) const;
    std::vector<ProcessStatus>  getAllStatus() const;
    std::string                 formatProcessInfo(const Process *proc) const;
    void                        shutdown();
};
