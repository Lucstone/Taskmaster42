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

void ProcessManager::loadConfig(const std::string &path) {
    _configs = ConfigParser::loadAll(path);

    for (const auto &[name, config] : _configs) {
        if (_table.find(name) == _table.end()) {
            _table[name] = std::vector<ProcessInfo>();
            _table[name].push_back(ProcessInfo(-1));
        }

        if (config.getAutostart()) {
            startProgram(name);
        }
    }
}

void ProcessManager::handle_status(const std::vector<std::string>& args) {
    if (args.size() == 1)
        std::cout << "[ProcessManager] Showing status for all programs\n";
    else {
        std::cout << "[ProcessManager] Showing status for specific programs:\n";
        for (size_t i = 1; i < args.size(); ++i)
            std::cout << "  - " << args[i] << "\n";
    }
}

void ProcessManager::handle_start(const std::vector<std::string>& args) {
    if (args.size() == 1)
        std::cout << "[ProcessManager] Starting all programs\n";
    else {
        std::cout << "[ProcessManager] Starting specific programs:\n";
        for (size_t i = 1; i < args.size(); ++i)
            std::cout << "  - " << args[i] << "\n";
    }
}

void ProcessManager::handle_stop(const std::vector<std::string>& args) {
    if (args.size() == 1)
        std::cout << "[ProcessManager] Stopping all programs\n";
    else {
        std::cout << "[ProcessManager] Stopping specific programs:\n";
        for (size_t i = 1; i < args.size(); ++i)
            std::cout << "  - " << args[i] << "\n";
    }
}

void ProcessManager::handle_restart(const std::vector<std::string>& args) {
    if (args.size() == 1)
        std::cout << "[ProcessManager] Restarting all programs\n";
    else {
        std::cout << "[ProcessManager] Restarting specific programs:\n";
        for (size_t i = 1; i < args.size(); ++i)
            std::cout << "  - " << args[i] << "\n";
    }
}

void ProcessManager::handle_reload() {
    std::cout << "[ProcessManager] Reloading configuration...\n";
}

//===================================================================

// -------------------- Gestion des processus --------------------
/*std::vector<pid_t> ProcessManager::startProgram(const std::string& name) {
    std::vector<pid_t> pids;

    auto it = _configs.find(name);
    if (it == _configs.end()) return pids;

    const ConfigParser& cfg = it->second;
    int N = std::max(1, cfg.getNumprocs());

    // Assure qu'il y a au moins un ProcessInfo
    if (_table[name].empty()) _table[name].push_back(ProcessInfo(-1));

    for (int i = 0; i < N; ++i) {
        pid_t pid = spawnOne(cfg);
        if (pid > 0) {
            ProcessInfo info(pid);
            info.markStartedNow();

            if (i < (int)_table[name].size()) {
                _table[name][i] = info; // remplacer si existant
            } else {
                _table[name].push_back(info); // sinon ajouter
            }

            pids.push_back(pid);
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
    if (_table.empty()) return;

    // Calcul du max length des noms
    size_t maxNameLen = 0;
    for (const auto& kv : _table)
        maxNameLen = std::max(maxNameLen, kv.first.size());

    const size_t MIN_WIDTH = 33;
    size_t colWidth = std::max(maxNameLen, MIN_WIDTH);

    // Tri des noms ASCII
    std::vector<std::string> names;
    for (const auto& kv : _table) names.push_back(kv.first);
    std::sort(names.begin(), names.end());

    // Fonction format uptime
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

    // Fonction format stop time
    auto formatStopTime = [](time_t stoppedAt) -> std::string {
        if (stoppedAt == 0) return "Not started";
        std::tm* tm_ptr = std::localtime(&stoppedAt);
        char buf[64];
        std::strftime(buf, sizeof(buf), "%b %d %I:%M %p", tm_ptr);
        return std::string(buf);
    };

    for (const std::string& name : names) {
        const auto& processes = _table.at(name);
        if (processes.empty()) continue;

        const ProcessInfo& pinfo = processes.back();

        std::string state;
        std::ostringstream extra;

        switch (pinfo.getState()) {
            case ProcessInfo::STARTING:
                state = "STARTING";
                break;
            case ProcessInfo::RUNNING:
                state = "RUNNING";
                extra << "pid " << std::setw(6) << pinfo.getPid()
                      << ", uptime " << formatUptime(pinfo.getStartedAt());
                break;
            case ProcessInfo::STOPPED:
            default:
                state = "STOPPED";
                if (!pinfo.everStarted())
                    extra << "Not started";
                else
                    extra << formatStopTime(pinfo.getStoppedAt());
                break;
        }

        std::cout << std::left << std::setw(colWidth + 3) << name
                  << std::setw(10) << state
                  << extra.str()
                  << std::endl;
    }
}

void ProcessManager::logEvent(const std::string& type, const std::string& name, pid_t pid, int code) {
    std::cout << "[" << std::time(nullptr) << "] " << type
              << " name=" << name
              << " pid=" << pid
              << " code=" << code << "\n";
}
*/