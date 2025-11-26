#include "ProgramConfig.hpp"
#include "../utils/Utils.hpp"
#include <algorithm>

ProgramConfig::ProgramConfig() 
    : _name(""),
      _cmd(""),
      _numprocs(1),
      _umask(022),
      _workingdir("/tmp"),
      _autostart(false),
      _autorestart("never"),
      _exitcodes(),
      _startretries(3),
      _starttime(1),
      _stopsignal("TERM"),
      _stoptime(10),
      _stdout_file(""),
      _stderr_file(""),
      _env() {
    _exitcodes.push_back(0);
}

ProgramConfig::ProgramConfig(const std::string &name)
    : _name(name),
      _cmd(""),
      _numprocs(1),
      _umask(022),
      _workingdir("/tmp"),
      _autostart(false),
      _autorestart("never"),
      _exitcodes(),
      _startretries(3),
      _starttime(1),
      _stopsignal("TERM"),
      _stoptime(10),
      _stdout_file(""),
      _stderr_file(""),
      _env() {
    _exitcodes.push_back(0);
}

ProgramConfig::ProgramConfig(const ProgramConfig &other)
    : _name(other._name),
      _cmd(other._cmd),
      _numprocs(other._numprocs),
      _umask(other._umask),
      _workingdir(other._workingdir),
      _autostart(other._autostart),
      _autorestart(other._autorestart),
      _exitcodes(other._exitcodes),
      _startretries(other._startretries),
      _starttime(other._starttime),
      _stopsignal(other._stopsignal),
      _stoptime(other._stoptime),
      _stdout_file(other._stdout_file),
      _stderr_file(other._stderr_file),
      _env(other._env) {
}

ProgramConfig &ProgramConfig::operator=(const ProgramConfig &other) {
    if (this != &other) {
        _name = other._name;
        _cmd = other._cmd;
        _numprocs = other._numprocs;
        _umask = other._umask;
        _workingdir = other._workingdir;
        _autostart = other._autostart;
        _autorestart = other._autorestart;
        _exitcodes = other._exitcodes;
        _startretries = other._startretries;
        _starttime = other._starttime;
        _stopsignal = other._stopsignal;
        _stoptime = other._stoptime;
        _stdout_file = other._stdout_file;
        _stderr_file = other._stderr_file;
        _env = other._env;
    }
    return *this;
}

ProgramConfig::~ProgramConfig() {
}

bool ProgramConfig::isExpectedExitCode(int code) const {
    return std::find(_exitcodes.begin(), _exitcodes.end(), code) != _exitcodes.end();
}

int ProgramConfig::getStopSignalNumber() const {
    return Utils::signalNameToNumber(_stopsignal);
}

bool ProgramConfig::shouldRestart(int exitCode) const {
    if (_autorestart == "always") {
        return true;
    } else if (_autorestart == "never") {
        return false;
    } else if (_autorestart == "unexpected") {
        return !isExpectedExitCode(exitCode);
    }
    return false;
}

bool ProgramConfig::operator==(const ProgramConfig &other) const {
    return _name == other._name &&
           _cmd == other._cmd &&
           _numprocs == other._numprocs &&
           _umask == other._umask &&
           _workingdir == other._workingdir &&
           _autostart == other._autostart &&
           _autorestart == other._autorestart &&
           _exitcodes == other._exitcodes &&
           _startretries == other._startretries &&
           _starttime == other._starttime &&
           _stopsignal == other._stopsignal &&
           _stoptime == other._stoptime &&
           _stdout_file == other._stdout_file &&
           _stderr_file == other._stderr_file &&
           _env == other._env;
}

bool ProgramConfig::operator!=(const ProgramConfig &other) const {
    return !(*this == other);
}
