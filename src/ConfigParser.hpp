/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 06:36:06 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/04 06:28:19 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <exception>
#include <yaml-cpp/yaml.h>

class ConfigParser {
public:
    class InvalidException : public std::exception {
    public:
        const char* what() const noexcept override {
            return "Missing or invalid section in config";
        }
    };

    ConfigParser() = default;
    ConfigParser(const ConfigParser &other) = default;
    ConfigParser &operator=(const ConfigParser &other) = default;
    ~ConfigParser() = default;

    const std::string                           &getName() const { return _name; }
    const std::string                           &getCmd() const { return _cmd; }
    const std::string                           &getUmask() const { return _umask; }
    const std::string                           &getAutorestart() const { return _autorestart; }
    const std::string                           &getStopsignals() const { return _stopsignals; }
    const std::string                           &getStdoutPath() const { return _stdout; }
    const std::string                           &getStderrPath() const { return _stderr; }
    const std::string                           &getWorkingDir() const { return _workingdir; }
    const std::vector<int>                      &getExitcodes() const { return _exitcodes; }
    const std::map<std::string,std::string>     &getEnv() const { return _env; }
    bool                                        getAutostart() const { return _autostart; }
    int                                         getNumProcs() const { return _numprocs; }
    int                                         getStartRetries() const { return _startretries; }
    int                                         getStartTime() const { return _starttime; }
    int                                         getStopTime() const { return _stoptime; }

    void                                        setName(const std::string& val) { _name = val; }
    void                                        setCmd(const std::string& val) { _cmd = val; }
    void                                        setUmask(const std::string& val) { _umask = val; }
    void                                        setAutorestart(const std::string& val) { _autorestart = val; }
    void                                        setStopsignals(const std::string& val) { _stopsignals = val; }
    void                                        setStdoutPath(const std::string& val) { _stdout = val; }
    void                                        setStderrPath(const std::string& val) { _stderr = val; }
    void                                        setWorkingDir(const std::string& val) { _workingdir = val; }
    void                                        setExitcodes(const std::vector<int>& val) { _exitcodes = val; }
    void                                        setEnv(const std::map<std::string,std::string>& val) { _env = val; }
    void                                        setAutostart(bool val) { _autostart = val; }
    void                                        setNumProcs(int val) { _numprocs = val; }
    void                                        setStartRetries(int val) { _startretries = val; }
    void                                        setStartTime(int val) { _starttime = val; }
    void                                        setStopTime(int val) { _stoptime = val; }

    static ConfigParser                         fromYAML(const std::string& name, const YAML::Node& node);
    static std::map<std::string, ConfigParser>  loadAll(const std::string& path);

private:
    std::string                         _name{};
    std::string                         _cmd{};
    std::string                         _umask{"022"};
    std::string                         _autorestart{"unexpected"};
    std::string                         _stopsignals{"TERM"};
    std::string                         _stdout{};
    std::string                         _stderr{};
    std::string                         _workingdir{};
    std::vector<int>                    _exitcodes{0, 1};
    std::map<std::string, std::string>  _env{};
    bool                                _autostart{false};
    int                                 _numprocs{1};
    int                                 _startretries{3};
    int                                 _starttime{5};
    int                                 _stoptime{10};
};
