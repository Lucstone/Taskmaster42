#pragma once

#include <string>
#include <vector>
#include <map>

class ProgramConfig {
private:
    std::string                         _name;
    std::string                         _cmd;
    int                                 _numprocs;
    int                                 _umask;
    std::string                         _workingdir;
    bool                                _autostart;
    std::string                         _autorestart;
    std::vector<int>                    _exitcodes;
    int                                 _startretries;
    int                                 _starttime;
    std::string                         _stopsignal;
    int                                 _stoptime;
    std::string                         _stdout_file;
    std::string                         _stderr_file;
    std::map<std::string, std::string>  _env;

public:
    ProgramConfig();
    ProgramConfig(const std::string &name);
    ProgramConfig(const ProgramConfig &other);
    ProgramConfig &operator=(const ProgramConfig &other);
    ~ProgramConfig();

    const std::string                           &getName() const { return _name; }
    const std::string                           &getCmd() const { return _cmd; }
    int                                         getNumprocs() const { return _numprocs; }
    int                                         getUmask() const { return _umask; }
    const std::string                           &getWorkingdir() const { return _workingdir; }
    bool                                        getAutostart() const { return _autostart; }
    const std::string                           &getAutorestart() const { return _autorestart; }
    const std::vector<int>                      &getExitcodes() const { return _exitcodes; }
    int                                         getStartretries() const { return _startretries; }
    int                                         getStarttime() const { return _starttime; }
    const std::string                           &getStopsignal() const { return _stopsignal; }
    int                                         getStoptime() const { return _stoptime; }
    const std::string                           &getStdoutFile() const { return _stdout_file; }
    const std::string                           &getStderrFile() const { return _stderr_file; }
    const std::map<std::string, std::string>    &getEnv() const { return _env; }

    void                                        setName(const std::string &name) { _name = name; }
    void                                        setCmd(const std::string &cmd) { _cmd = cmd; }
    void                                        setNumprocs(int numprocs) { _numprocs = numprocs; }
    void                                        setUmask(int umask) { _umask = umask; }
    void                                        setWorkingdir(const std::string &dir) { _workingdir = dir; }
    void                                        setAutostart(bool autostart) { _autostart = autostart; }
    void                                        setAutorestart(const std::string &autorestart) { _autorestart = autorestart; }
    void                                        setExitcodes(const std::vector<int> &codes) { _exitcodes = codes; }
    void                                        setStartretries(int retries) { _startretries = retries; }
    void                                        setStarttime(int time) { _starttime = time; }
    void                                        setStopsignal(const std::string &signal) { _stopsignal = signal; }
    void                                        setStoptime(int time) { _stoptime = time; }
    void                                        setStdoutFile(const std::string &file) { _stdout_file = file; }
    void                                        setStderrFile(const std::string &file) { _stderr_file = file; }
    void                                        setEnv(const std::map<std::string, std::string> &env) { _env = env; }

    bool                                        isExpectedExitCode(int code) const;
    int                                         getStopSignalNumber() const;
    bool                                        shouldRestart(int exitCode) const;

    bool operator==(const ProgramConfig &other) const;
    bool operator!=(const ProgramConfig &other) const;
};
