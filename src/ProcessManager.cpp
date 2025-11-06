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
#include <iostream>
#include <iomanip>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <thread>
#include <chrono>

// -------------------- Config --------------------
void ProcessManager::loadConfig(const std::string& path) {
    _configs = ProgramConfig::loadAll(path);
}

void ProcessManager::reloadConfig(const std::string& path) {
    auto fresh = ProgramConfig::loadAll(path);

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

    const ProgramConfig& cfg = it->second;
    int N = std::max(1, cfg.getNumprocs());

    for (int i = 0; i < N; ++i) {
        pid_t pid = spawnOne(cfg);
        if (pid > 0) {
            ProcessInfo info(pid);
            info.markStartedNow();
            _table[name].push_back(info);
            pids.push_back(pid);
            logEvent("start", name, pid, 0);
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

    const ProgramConfig& cfg = cit->second;
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
pid_t ProcessManager::spawnOne(const ProgramConfig& cfg) {
    return _launcher.spawnOne(cfg); // délégation
}

// -------------------- Logs / Status --------------------
void ProcessManager::printStatus() const {
    std::cout << "NAME        STATE      PID       RESTARTS\n";
    for (auto& [name, vec] : _table) {
        if (vec.empty()) {
            std::cout << std::left << std::setw(12) << name
                      << std::setw(11) << "STOPPED"
                      << std::setw(10) << "-"
                      << "-" << "\n";
            continue;
        }
        for (auto& p : vec) {
            bool running = p.isRunning();
            std::cout << std::left << std::setw(12) << name
                      << std::setw(11) << (running ? "RUNNING" : "STOPPED")
                      << std::setw(10) << (running ? std::to_string(p.getPid()) : "-")
                      << p.getRestarts()
                      << "\n";
        }
    }
}

void ProcessManager::logEvent(const std::string& type, const std::string& name, pid_t pid, int code) {
    std::cout << "[" << std::time(nullptr) << "] " << type
              << " name=" << name
              << " pid=" << pid
              << " code=" << code << "\n";
}
