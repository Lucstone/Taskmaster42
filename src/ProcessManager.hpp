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
#include <map>
#include <string>
#include <vector>
#include "ProcessInfo.hpp"
#include "ConfigParser.hpp"
#include "ProcessLauncher.hpp"

class ProcessManager {
public:
    ProcessManager() = default;
    ~ProcessManager() = default;

    // Config
    void loadConfig(const std::string& path);
    void reloadConfig(const std::string& path);

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
    pid_t spawnOne(const ProgramConfig& cfg);

    bool findByPid(pid_t pid, std::string& nameOut, size_t& idxOut);

    std::map<std::string, ProgramConfig> _configs;
    std::map<std::string, std::vector<ProcessInfo>> _table;

    ProcessLauncher _launcher; // <-- délégation de fork/exec/redirections
};
