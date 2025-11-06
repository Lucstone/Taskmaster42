#pragma once
#include "ProcessInfo.hpp"
#include "ConfigParser.hpp"
#include "ProcessLauncher.hpp"
#include "Logger.hpp"
#include <map>
#include <vector>
#include <string>

class ProcessSupervisor {
private:
    std::map<std::string, std::vector<ProcessInfo>> _table;
    ProcessLauncher* _launcher = nullptr;

public:
    ProcessSupervisor(ProcessLauncher* launcher) : _launcher(launcher) {}

    std::vector<pid_t> startProgram(const ProgramConfig& cfg, const std::string& name);
    void stopProgram(const ProgramConfig& cfg, const std::string& name);
    void restartProgram(const ProgramConfig& cfg, const std::string& name);

    void startAutostartPrograms(const std::map<std::string, ProgramConfig>& configs);
    void tick();
    void handleExit(pid_t pid, int status);
    void printStatus() const;
};
