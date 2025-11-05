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
#include "ProgramConfig.hpp"
#include "ProcInfo.hpp"

#include <map>
#include <vector>
#include <string>

class ProcessManager {
private:
    // name -> config chargée depuis config.yaml
    std::map<std::string, ProgramConfig> _configs;
    // name -> instances en cours
    std::map<std::string, std::vector<ProcInfo>> _table;

    // Lancement d'une instance (child setup + exec)
    pid_t spawnOne(const ProgramConfig& cfg);

    // Trouver (name, index) à partir d'un pid récolté par waitpid
    bool findByPid(pid_t pid, std::string& nameOut, size_t& idxOut);

    // Applique la politique de restart quand un process meurt
    void handleExit(pid_t pid, int status);

public:
    ProcessManager() {}
    ~ProcessManager() {}

    // Chargement / reload de la config
    void loadConfig(const std::string& path);
    void reloadConfig(const std::string& path); // simple pour commencer

    // Commandes de base (utilisées par le shell)
    std::vector<pid_t> startProgram(const std::string& name);
    void stopProgram(const std::string& name);
    void restartProgram(const std::string& name);

    // Autostart
    void startAutostartPrograms();

    // Surveillance non bloquante
    void tick();

    // Affichage status (pratique pour tests/shell)
    void printStatus() const;

    // Logging (stdout pour commencer)
    void logEvent(const std::string& type, const std::string& name, pid_t pid, int code = 0);
};
