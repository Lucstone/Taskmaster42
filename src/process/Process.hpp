#pragma once

#include "ProcessState.hpp"
#include "../config/ProgramConfig.hpp"
#include <sys/types.h>
#include <string>
#include <ctime>

class Process {
private:
    ProgramConfig _config;
    std::string _instance_name; // e.g., "ls_cat_0" for first instance
    ProcessState _state;
    pid_t _pid;
    time_t _start_time;
    int _restart_count;
    int _exit_code;
    time_t _stop_time; // When stop signal was sent
    
    // Internal methods
    bool forkAndExec();
    void setupChildEnvironment();
    void redirectOutputs();
    bool hasRunLongEnough() const;
    void sendStopSignal();

public:
    // Default constructor
    Process();
    
    // Parameterized constructor
    Process(const ProgramConfig& config, int instance_num);
    
    // Copy constructor
    Process(const Process& other);
    
    // Copy assignment operator
    Process& operator=(const Process& other);
    
    // Destructor
    ~Process();
    
    // Lifecycle management
    bool start();
    bool stop();
    bool kill(); // Force kill with SIGKILL
    void restart();
    
    // State queries
    ProcessState getState() const { return _state; }
    pid_t getPid() const { return _pid; }
    const std::string& getName() const { return _instance_name; }
    const std::string& getProgramName() const { return _config.getName(); }
    bool isRunning() const;
    int getRestartCount() const { return _restart_count; }
    int getExitCode() const { return _exit_code; }
    time_t getUptime() const;
    
    // Called when SIGCHLD received
    void handleProcessExit(int exit_status);
    
    // Check if process should transition from STARTING to RUNNING
    // Also handles stop timeout (STOPPING -> SIGKILL)
    void updateState();
    
    // Get status string for display
    std::string getStatusString() const;
};