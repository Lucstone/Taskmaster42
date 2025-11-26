#include "Process.hpp"
#include "../logger/Logger.hpp"
#include "../utils/Utils.hpp"
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <sstream>
#include <vector>

Process::Process()
    : _config(),
      _instance_name(""),
      _state(ProcessState::STOPPED),
      _pid(-1),
      _start_time(0),
      _restart_count(0),
      _exit_code(0),
      _stop_time(0) {
}

Process::Process(const ProgramConfig &config, int instance_num)
    : _config(config),
      _instance_name(""),
      _state(ProcessState::STOPPED),
      _pid(-1),
      _start_time(0),
      _restart_count(0),
      _exit_code(0),
      _stop_time(0) {
    if (config.getNumprocs() == 1) {
        _instance_name = config.getName();
    } else {
        std::ostringstream  oss;

        oss << config.getName() << "_" << std::setw(2) << std::setfill('0') << instance_num;
        _instance_name = oss.str();
    }
}

Process::Process(const Process &other)
    : _config(other._config),
      _instance_name(other._instance_name),
      _state(other._state),
      _pid(other._pid),
      _start_time(other._start_time),
      _restart_count(other._restart_count),
      _exit_code(other._exit_code),
      _stop_time(other._stop_time) {
}

Process &Process::operator=(const Process &other) {
    if (this != &other) {
        _config = other._config;
        _instance_name = other._instance_name;
        _state = other._state;
        _pid = other._pid;
        _start_time = other._start_time;
        _restart_count = other._restart_count;
        _exit_code = other._exit_code;
        _stop_time = other._stop_time;
    }
    return *this;
}

Process::~Process() {
    if (_pid > 0 && _state != ProcessState::STOPPED) {
        stop();
    }
}

bool Process::start() {
    LOG_INFO("Starting process: " + _instance_name);

    if (_state == ProcessState::RUNNING || _state == ProcessState::STARTING) {
        LOG_WARNING("Process already running: " + _instance_name);
        return false;
    }

    _state = ProcessState::STARTING;
    _start_time = time(NULL);
    _restart_count++;

    bool success = forkAndExec();

    if (success) {
        LOG_INFO("Process started: " + _instance_name + " PID: " + std::to_string(_pid));
        return true;
    } else {
        LOG_ERROR("Failed to start process: " + _instance_name);

        if (!Utils::fileExists(_config.getCmd().substr(0, _config.getCmd().find(' ')))) {
            _state = ProcessState::BACKOFF;
        } else {
            _state = ProcessState::FATAL;
        }
        return false;
    }
}

bool Process::forkAndExec() {
    int stdin_pipe[2];

    if (pipe(stdin_pipe) != 0) {
        return false;
    }

    pid_t pid = fork();
    if (pid < 0) {
        LOG_ERROR("Fork failed: " + std::string(strerror(errno)));
        close(stdin_pipe[0]);
        close(stdin_pipe[1]);
        return false;
    }

    if (pid == 0) {
        close(stdin_pipe[1]);
        dup2(stdin_pipe[0], STDIN_FILENO);
        close(stdin_pipe[0]);

        setupChildEnvironment();
        redirectOutputs();

        std::vector<std::string>    tokens = Utils::split(_config.getCmd(), ' ');

        if (tokens.empty()) {
            _exit(1);
        }

        std::vector<char*>  argv;

        for (size_t i = 0; i < tokens.size(); ++i) {
            argv.push_back(const_cast<char*>(tokens[i].c_str()));
        }
        argv.push_back(NULL);

        execvp(argv[0], &argv[0]);

        std::cerr << "execv failed: " << strerror(errno) << "\n";
        _exit(1);
    }

    _pid = pid;
    close(stdin_pipe[0]);
    return true;
}

void Process::setupChildEnvironment() {
    if (!_config.getWorkingdir().empty()) {
        if (chdir(_config.getWorkingdir().c_str()) != 0) {
            std::cerr << "chdir failed: " << strerror(errno) << "\n";
        }
    }

    umask(_config.getUmask());

    const std::map<std::string, std::string>                &env = _config.getEnv();
    for (std::map<std::string, std::string>::const_iterator it = env.begin();
         it != env.end(); ++it) {
        setenv(it->first.c_str(), it->second.c_str(), 1);
    }
}

void Process::redirectOutputs() {
    if (!_config.getStdoutFile().empty()) {
        int fd = open(_config.getStdoutFile().c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);

        if (fd >= 0) {
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
    }

    if (!_config.getStderrFile().empty()) {
        int fd = open(_config.getStderrFile().c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);

        if (fd >= 0) {
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
    }
}

bool Process::hasRunLongEnough() const {
    if (_start_time == 0) {
        return false;
    }
    return (time(NULL) - _start_time) >= _config.getStarttime();
}

void Process::sendStopSignal() {
    if (_pid > 0) {
        int sig = _config.getStopSignalNumber();

        LOG_INFO("Sending signal " + Utils::signalNumberToName(sig) + " to process: " + _instance_name);
        ::kill(_pid, sig);
    }
}

bool Process::stop() {
    LOG_INFO("Stopping process: " + _instance_name);

    if (_pid <= 0 || _state == ProcessState::STOPPED) {
        return true;
    }

    _state = ProcessState::STOPPING;
    _stop_time = time(NULL);

    sendStopSignal();

    return true;
}

bool Process::kill() {
    LOG_INFO("Killing process: " + _instance_name);

    if (_pid > 0) {
        ::kill(_pid, SIGKILL);
        _state = ProcessState::STOPPED;
        _pid = -1;
    }

    return true;
}

void Process::restart() {
    LOG_INFO("Restarting process: " + _instance_name);

    if (_state == ProcessState::RUNNING || _state == ProcessState::STARTING) {
        stop();
        sleep(1);
    }

    start();
}

bool Process::isRunning() const {
    return (_state == ProcessState::STARTING || 
            _state == ProcessState::RUNNING);
}

time_t Process::getUptime() const {
    if (_start_time == 0 || !isRunning()) {
        return 0;
    }
    return time(NULL) - _start_time;
}

void Process::handleProcessExit(int exit_status) {
    if (WIFEXITED(exit_status)) {
        _exit_code = WEXITSTATUS(exit_status);
        LOG_INFO("Process exited with code " + std::to_string(_exit_code) + ": " + _instance_name);
    } else if (WIFSIGNALED(exit_status)) {
        int sig = WTERMSIG(exit_status);

        LOG_INFO("Process killed by signal " + Utils::signalNumberToName(sig) + ": " + _instance_name);
        _exit_code = 128 + sig;
    }

    _pid = -1;

    if (_state == ProcessState::STOPPING) {
        _state = ProcessState::STOPPED;
        LOG_INFO("Process stopped as requested: " + _instance_name);
        return;
    }

    if (_state == ProcessState::STARTING && !hasRunLongEnough()) {
        LOG_ERROR("Process exited too quickly: " + _instance_name);

        if (_restart_count <= _config.getStartretries()) {
            _state = ProcessState::BACKOFF;
        } else {
            _state = ProcessState::FATAL;
            LOG_ERROR("Max restart attempts reached: " + _instance_name);
        }
        return;
    }

    if (_config.shouldRestart(_exit_code)) {
        if (_restart_count <= _config.getStartretries()) {
            _state = ProcessState::BACKOFF;
            LOG_INFO("Process will be restarted: " + _instance_name);
        } else {
            _state = ProcessState::FATAL;
            LOG_ERROR("Max restart attempts reached: " + _instance_name);
        }
    } else {
        _state = ProcessState::EXITED;
        LOG_INFO("Process exited (expected): " + _instance_name);
    }
}

void Process::updateState() {
    if (_state == ProcessState::STARTING && hasRunLongEnough()) {
        _state = ProcessState::RUNNING;
        _restart_count = 0;
        LOG_INFO("Process now running: " + _instance_name);
    }

    if (_state == ProcessState::STOPPING && _pid > 0) {
        time_t  elapsed = time(NULL) - _stop_time;

        if (elapsed >= _config.getStoptime()) {
            LOG_WARNING("Process stop timeout (" + std::to_string(_config.getStoptime()) + "s), sending SIGKILL: " + _instance_name);
            kill();
        }
    }

    if (_state == ProcessState::BACKOFF) {
        LOG_INFO("Retrying start (attempt " + std::to_string(_restart_count + 1) + "): " + _instance_name);
        start();
    }
}

std::string Process::getStatusString() const {
    std::ostringstream  oss;

    oss << _instance_name << " " << processStateToString(_state);

    if (_state == ProcessState::RUNNING && _pid > 0) {
        oss << " pid " << _pid << ", uptime " << getUptime() << "s";
    }

    return oss.str();
}
