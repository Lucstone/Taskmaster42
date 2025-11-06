/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProcessManager1.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 06:36:14 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/05 07:15:53 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ProcessManager.hpp"
#include "Utils.hpp"
#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <thread>
#include <chrono>
#include <ctime>

// -------------------- Config --------------------
void ProcessManager::loadConfig(const std::string& path) {
    _configs = ConfigParser::loadAll(path);
}

void ProcessManager::reloadConfig(const std::string& path) {
    auto fresh = ConfigParser::loadAll(path);

    for (auto it = _configs.begin(); it != _configs.end(); ++it) {
        if (!fresh.count(it->first)) {
            stopProgram(it->first);
            logEvent("removed", it->first, -1, 0);
        }
    }

    _configs.swap(fresh);
    startAutostartPrograms();
    logEvent("reload", "-", -1, 0);
}

// -------------------- Gestion des processus --------------------
std::vector<pid_t> ProcessManager::startProgram(const std::string& name) {
    std::vector<pid_t> pids;

    auto it = _configs.find(name);
    if (it == _configs.end()) {
        std::cerr << "[start] unknown program: " << name << "\n";
        return pids;
    }

    const ConfigParser& cfg = it->second;
    int N = std::max(1, cfg.getNumprocs());

    for (int i = 0; i < N; ++i) {
        pid_t pid = spawnOne(cfg);
        if (pid > 0) {
            ProcessInfo info(pid);
            info.markStartedNow();
            _table[name].push_back(info);
            pids.push_back(pid);
        } else {
            std::cerr << "[start] failed: " << name << " instance " << i << "\n";
        }
    }
    return pids;
}

void ProcessManager::stopProgram(const std::string& name) {
    auto cit = _configs.find(name);
    auto tit = _table.find(name);
    if (cit == _configs.end() || tit == _table.end()) return;

    const ConfigParser& cfg = cit->second;
    std::vector<ProcessInfo>& procs = tit->second;
    int sig = SIGTERM; // on peut utiliser signalFromString si nécessaire

    for (auto& pinfo : procs) {
        if (pinfo.getPid() > 0) ::kill(pinfo.getPid(), sig);
    }

    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(cfg.getStoptime());

    for (auto& pinfo : procs) {
        pid_t p = pinfo.getPid();
        if (p <= 0) continue;

        while (true) {
            int st = 0;
            pid_t r = ::waitpid(p, &st, WNOHANG);
            if (r == p) { pinfo.markStopped(); logEvent("stop", name, p, 0); break; }

            if (std::chrono::steady_clock::now() >= deadline) {
                ::kill(p, SIGKILL);
                ::waitpid(p, &st, 0);
                pinfo.markStopped();
                logEvent("killed", name, p, 0);
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void ProcessManager::restartProgram(const std::string& name) {
    stopProgram(name);
    startProgram(name);
}

void ProcessManager::startAutostartPrograms() {
    for (auto& [name, cfg] : _configs) {
        if (cfg.getAutostart()) startProgram(name);
    }
}

// -------------------- Surveillance --------------------
void ProcessManager::tick() {
    int status = 0;
    while (true) {
        pid_t pid = ::waitpid(-1, &status, WNOHANG);
        if (pid <= 0) break;
        handleExit(pid, status);
    }
}

bool ProcessManager::findByPid(pid_t pid, std::string& nameOut, size_t& idxOut) {
    for (auto& [name, vec] : _table) {
        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i].getPid() == pid) {
                nameOut = name;
                idxOut = i;
                return true;
            }
        }
    }
    return false;
}

void ProcessManager::handleExit(pid_t pid, int status) {
    std::string name;
    size_t idx = 0;
    if (!findByPid(pid, name, idx)) return;

    int exitCode = -1;
    if (WIFEXITED(status)) exitCode = WEXITSTATUS(status);

    logEvent("exit", name, pid, exitCode);
}

// -------------------- Lancement via ProcessLauncher --------------------
pid_t ProcessManager::spawnOne(const ConfigParser& cfg) {
    return _launcher.spawnOne(cfg); // délégation
}

// -------------------- Logs / Status --------------------
void ProcessManager::printStatus() const {
    if (_table.empty()) {
        std::cout << "No programs loaded.\n";
        return;
    }

    // Obtenir la taille max des noms
    size_t maxNameLen = 0;
    for (const auto& kv : _table)
        maxNameLen = std::max(maxNameLen, kv.first.size());

    // Largeur minimale (comme Supervisor)
    const size_t MIN_WIDTH = 33;
    size_t colWidth = std::max(maxNameLen, MIN_WIDTH);

    // Trier les programmes par ordre ASCII
    std::vector<std::string> names;
    for (const auto& kv : _table) names.push_back(kv.first);
    std::sort(names.begin(), names.end());

    // Calcul du temps d'uptime
    auto formatUptime = [](time_t startedAt) -> std::string {
        if (startedAt == 0) return "0:00:00";
        int diff = static_cast<int>(std::time(nullptr) - startedAt);
        int hours = diff / 3600;
        int mins  = (diff % 3600) / 60;
        int secs  = diff % 60;
        std::ostringstream oss;
        oss << hours << ":" << std::setw(2) << std::setfill('0') << mins
            << ":" << std::setw(2) << std::setfill('0') << secs;
        return oss.str();
    };

    // Affichage des lignes
    for (const std::string& name : names) {
        const auto& processes = _table.at(name);
        if (processes.empty()) continue;

        const ProcessInfo& pinfo = processes.back();

        std::cout << std::left << std::setw(colWidth)
                  << name
                  << (pinfo.isRunning() ? "RUNNING   " : "STOPPED   ")
                  << "pid " << std::setw(6) << pinfo.getPid()
                  << ", uptime " << formatUptime(pinfo.getStartedAt())
                  << "\n";
    }
}

void ProcessManager::logEvent(const std::string& type, const std::string& name, pid_t pid, int code) {
    std::cout << "[" << std::time(nullptr) << "] " << type
              << " name=" << name
              << " pid=" << pid
              << " code=" << code << "\n";
}
