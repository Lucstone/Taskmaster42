#pragma once

#include "ProcessState.hpp"
#include "../config/ProgramConfig.hpp"
#include <sys/types.h>
#include <string>
#include <ctime>

class Process {
private:
    ProgramConfig   _config;
    std::string     _instance_name;
    ProcessState    _state;
    pid_t           _pid;
    time_t          _start_time;
    int             _restart_count;
    int             _exit_code;
    time_t          _stop_time;

    bool                forkAndExec();
    void                setupChildEnvironment();
    void                redirectOutputs();
    bool                hasRunLongEnough() const;
    void                sendStopSignal();

public:
    Process();
    Process(const ProgramConfig &config, int instance_num); 
    Process(const Process &other);
    Process &operator=(const Process &other);
    ~Process();

    bool                start();
    bool                stop();
    bool                kill();
    void                restart();

    ProcessState        getState() const { return _state; }
    pid_t               getPid() const { return _pid; }
    const std::string   &getName() const { return _instance_name; }
    const std::string   &getProgramName() const { return _config.getName(); }
    bool                isRunning() const;
    int                 getRestartCount() const { return _restart_count; }
    int                 getExitCode() const { return _exit_code; }
    time_t              getUptime() const;

    void                handleProcessExit(int exit_status);
    void                updateState();
    std::string         getStatusString() const;
};
