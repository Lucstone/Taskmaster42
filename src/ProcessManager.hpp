/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessManager.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 06:36:12 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/05 06:03:39 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>
#include <vector>
#include <iostream>

#include "ConfigParser.hpp"

class ProcessManager
{
private:
    std::map<std::string, ConfigParser> _configs;

public:
    ProcessManager();
    ProcessManager(const ProcessManager &other);
    ProcessManager& operator=(const ProcessManager &other);
    ~ProcessManager();

    void loadConfig(const std::string &path);

    void handle_status(const std::vector<std::string> &args);
    void handle_start(const std::vector<std::string> &args);
    void handle_stop(const std::vector<std::string> &args);
    void handle_restart(const std::vector<std::string> &args);
    void handle_reload();
};


/*class ProcessManager {
public:
    ProcessManager() = default;
    ~ProcessManager() = default;

    // Gestion des processus
    std::vector<pid_t> startProgram(const std::string& name);
    void stopProgram(const std::string& name);
    void restartProgram(const std::string& name);
    void startAutostartPrograms();

    // Surveillance
    void tick();
    void handleExit(pid_t pid, int status);

    // Status / Logs
    void printStatus() const;
    void logEvent(const std::string& type, const std::string& name, pid_t pid, int code);

private:
    pid_t spawnOne(const ConfigParser& cfg);

    bool findByPid(pid_t pid, std::string& nameOut, size_t& idxOut);

    std::map<std::string, ConfigParser> _configs;
    std::map<std::string, std::vector<ProcessInfo>> _table;

    ProcessLauncher _launcher; // <-- délégation de fork/exec/redirections
};

class ProcessManager {
private:
    std::map<std::string, ConfigParser> _configs;
    std::map<std::string, std::vector<ProcessInfo>> _table;

public:
    void loadConfig(const std::string& path); // charge config + initialise _table + autostart

    void startAutostartPrograms();

    std::vector<pid_t> startProgram(const std::string& name);
    void stopProgram(const std::string& name);
    void restartProgram(const std::string& name);
    void printStatus() const;
};*/