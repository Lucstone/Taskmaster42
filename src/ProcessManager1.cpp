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

#include "../include/ProcessManager.hpp"


#include <unistd.h>      // fork, chdir, execvp, setsid, usleep
#include <sys/wait.h>    // waitpid
#include <sys/stat.h>    // umask
#include <fcntl.h>       // open
#include <csignal>       // SIG*
#include <cerrno>
#include <cstdlib>       // setenv, strtol
#include <cstring>       // strerror
#include <ctime>         // time, localtime
#include <iostream>
#include <iomanip>

// --------- Helpers locaux (pas besoin dans ProgramConfig pour commencer) ----------
static int strToOctalUmask(const std::string& u) {
    char* end = 0;
    long v = std::strtol(u.c_str(), &end, 8); // base 8
    if (end == u.c_str() || v < 0 || v > 0777) return 0022; // défaut raisonnable
    return static_cast<int>(v);
}

static int signalFromString(const std::string& s) {
    if (s == "TERM") return SIGTERM;
    if (s == "INT")  return SIGINT;
    if (s == "KILL") return SIGKILL;
    if (s == "HUP")  return SIGHUP;
    if (s == "USR1") return SIGUSR1;
    if (s == "USR2") return SIGUSR2;
    return SIGTERM;
}

// split très simple par espaces ; si tu passes "cmd" en séquence YAML plus tard c'est mieux
static void splitCmd(const std::string& cmd, std::vector<char*>& argvStore) {
    static thread_local std::vector<std::string> toks;
    toks.clear();

    std::string cur;
    for (size_t i = 0; i < cmd.size(); ++i) {
        unsigned char ch = static_cast<unsigned char>(cmd[i]);
        if (std::isspace(ch)) {
            if (!cur.empty()) { toks.push_back(cur); cur.clear(); }
        } else {
            cur.push_back(cmd[i]);
        }
    }
    if (!cur.empty()) toks.push_back(cur);

    argvStore.clear();
    argvStore.reserve(toks.size() + 1);
    for (size_t i = 0; i < toks.size(); ++i)
        argvStore.push_back(const_cast<char*>(toks[i].c_str()));
    argvStore.push_back(nullptr);
}

static int open_for_redirect(const std::string& path) {
    return ::open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
}

static std::string nowString() {
    std::time_t t = std::time(nullptr);
    char buf[64]; buf[0] = 0;
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return std::string(buf);
}

// ------------------------------ Config -----------------------------------------
void ProcessManager::loadConfig(const std::string& path) {
    _configs = ProgramConfig::loadAll(path);
}

void ProcessManager::reloadConfig(const std::string& path) {
    // version simple pour commencer : on arrête les supprimés, puis on remplace
    auto fresh = ProgramConfig::loadAll(path);

    // stop ceux qui n'existent plus
    for (std::map<std::string, ProgramConfig>::const_iterator it = _configs.begin();
         it != _configs.end(); ++it)
    {
        if (!fresh.count(it->first)) {
            stopProgram(it->first);
            logEvent("removed", it->first, -1, 0);
        }
    }
    _configs.swap(fresh);
    startAutostartPrograms();
    logEvent("reload", "-", -1, 0);
}

// ------------------------- Démarrer / Arrêter / Redémarrer ----------------------
std::vector<pid_t> ProcessManager::startProgram(const std::string& name) {
    std::vector<pid_t> pids;

    std::map<std::string, ProgramConfig>::const_iterator it = _configs.find(name);
    if (it == _configs.end()) {
        std::cerr << "[start] unknown program: " << name << "\n";
        return pids;
    }
    const ProgramConfig& cfg = it->second;

    int N = cfg.getNumprocs() > 0 ? cfg.getNumprocs() : 1;
    for (int i = 0; i < N; ++i) {
        pid_t pid = spawnOne(cfg);
        if (pid > 0) {
            ProcInfo info(pid);
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
    std::map<std::string, ProgramConfig>::const_iterator cit = _configs.find(name);
    std::map<std::string, std::vector<ProcInfo> >::iterator tit = _table.find(name);
    if (cit == _configs.end() || tit == _table.end()) return;

    const ProgramConfig& cfg = cit->second;
    std::vector<ProcInfo>& procs = tit->second;

    const int sig = signalFromString(cfg.getStopsignals());
    const int timeout = cfg.getStoptime();

    // 1) signal doux
    for (size_t i = 0; i < procs.size(); ++i) {
        pid_t p = procs[i].getPid();
        if (p > 0) ::kill(p, sig);
    }

    // 2) attendre jusqu'à timeout, puis SIGKILL si nécessaire
    time_t deadline = std::time(nullptr) + timeout;
    for (size_t i = 0; i < procs.size(); ++i) {
        pid_t p = procs[i].getPid();
        if (p <= 0) continue;

        while (true) {
            int st = 0;
            pid_t r = ::waitpid(p, &st, WNOHANG);
            if (r == p) {
                procs[i].markStopped();
                logEvent("stop", name, p, 0);
                break;
            }
            if (std::time(nullptr) >= deadline) {
                ::kill(p, SIGKILL);
                ::waitpid(p, &st, 0);
                procs[i].markStopped();
                logEvent("killed", name, p, 0);
                break;
            }
            usleep(100 * 1000);
        }
    }
}

void ProcessManager::restartProgram(const std::string& name) {
    stopProgram(name);
    startProgram(name);
}

void ProcessManager::startAutostartPrograms() {
    for (std::map<std::string, ProgramConfig>::const_iterator it = _configs.begin();
         it != _configs.end(); ++it)
    {
        if (it->second.getAutostart())
            startProgram(it->first);
    }
}

// ------------------------------ Surveillance -----------------------------------
void ProcessManager::tick() {
    int status = 0;
    while (true) {
        pid_t pid = ::waitpid(-1, &status, WNOHANG);
        if (pid <= 0) break;
        handleExit(pid, status);
    }
}

bool ProcessManager::findByPid(pid_t pid, std::string& nameOut, size_t& idxOut) {
    for (std::map<std::string, std::vector<ProcInfo> >::iterator it = _table.begin();
         it != _table.end(); ++it)
    {
        std::vector<ProcInfo>& vec = it->second;
        for (size_t i = 0; i < vec.size(); ++i) {
            if (vec[i].getPid() == pid) {
                nameOut = it->first;
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
    if (!findByPid(pid, name, idx)) {
        return; // inconnu
    }

    ProgramConfig& cfg = _configs[name];
    ProcInfo& proc = _table[name][idx];

    // logs de sortie
    int exitCode = -1, termSig = 0;
    if (WIFEXITED(status)) exitCode = WEXITSTATUS(status);
    if (WIFSIGNALED(status)) termSig = WTERMSIG(status);
    logEvent("exit", name, pid, (exitCode >= 0 ? exitCode : -termSig));

    // fenêtre starttime / retries
    const bool early = proc.diedTooEarly(cfg.getStarttime());
    if (early) {
        proc.incrementStartFailures();
        if (proc.hasExceededRetries(cfg.getStartretries())) {
            proc.markStopped();
            logEvent("giveup", name, pid, exitCode);
            return;
        }
        // sinon on peut tenter une relance si la policy l'autorise
    } else {
        proc.setStartFailures(0); // démarrage validé
    }

    // politique autorestart
    bool expected = false;
    if (WIFEXITED(status)) {
        // expected si code ∈ exitcodes
        const std::vector<int>& ex = cfg.getExitcodes();
        for (size_t i = 0; i < ex.size(); ++i)
            if (ex[i] == exitCode) { expected = true; break; }
    } else if (WIFSIGNALED(status)) {
        expected = false; // mort par signal => unexpected
    }

    const std::string& policy = cfg.getAutorestart();
    bool should = false;
    if (policy == "always")           should = true;
    else if (policy == "never")       should = false;
    else if (policy == "unexpected")  should = !expected;

    if (!should) {
        proc.markStopped();
        return;
    }

    // relance
    pid_t np = spawnOne(cfg);
    if (np > 0) {
        proc.setPid(np);
        proc.markStartedNow();
        proc.incrementRestarts();
        logEvent("restart", name, np, 0);
    } else {
        proc.markStopped();
        logEvent("restart_failed", name, pid, 0);
    }
}

// ------------------------------- Lancement -------------------------------------
pid_t ProcessManager::spawnOne(const ProgramConfig& cfg) {
    pid_t pid = ::fork();
    if (pid < 0) { std::perror("fork"); return -1; }

    if (pid == 0) {
        // === ENFANT ===
        ::setsid(); // nouveau groupe/session

        // workingdir
        if (!cfg.getWorkingDir().empty()) {
            if (::chdir(cfg.getWorkingDir().c_str()) == -1) {
                std::perror("chdir"); _exit(127);
            }
        }

        // umask
        ::umask(strToOctalUmask(cfg.getUmask()));

        // redirections
        if (!cfg.getStdoutPath().empty()) {
            int fd = open_for_redirect(cfg.getStdoutPath());
            if (fd == -1 || ::dup2(fd, STDOUT_FILENO) == -1) {
                std::perror("redirect stdout"); _exit(127);
            }
            if (fd > 2) ::close(fd);
        }
        if (!cfg.getStderrPath().empty()) {
            int fd = open_for_redirect(cfg.getStderrPath());
            if (fd == -1 || ::dup2(fd, STDERR_FILENO) == -1) {
                std::perror("redirect stderr"); _exit(127);
            }
            if (fd > 2) ::close(fd);
        }

        // env (override)
        const std::map<std::string,std::string>& env = cfg.getEnv();
        for (std::map<std::string,std::string>::const_iterator it = env.begin();
             it != env.end(); ++it)
        {
            ::setenv(it->first.c_str(), it->second.c_str(), 1);
        }

        // exec
        std::vector<char*> argv;
        splitCmd(cfg.getCmd(), argv); // simple ; tu pourras switcher sur YAML séquence
        if (argv.empty()) _exit(127);

        ::execvp(argv[0], &argv[0]);
        std::perror("execvp");
        _exit(127);
    }

    // === PARENT ===
    return pid;
}

// ------------------------------- Affichage & Logs -------------------------------
void ProcessManager::printStatus() const {
    std::cout << "NAME        STATE      PID       RESTARTS\n";
    for (std::map<std::string, std::vector<ProcInfo> >::const_iterator it = _table.begin();
         it != _table.end(); ++it)
    {
        const std::string& name = it->first;
        const std::vector<ProcInfo>& vec = it->second;
        if (vec.empty()) {
            std::cout << std::left << std::setw(12) << name
                      << std::setw(11) << "STOPPED"
                      << std::setw(10) << "-"
                      << "-" << "\n";
            continue;
        }
        for (size_t i = 0; i < vec.size(); ++i) {
            const ProcInfo& p = vec[i];
            const bool running = p.isRunning();
            std::cout << std::left << std::setw(12) << name
                      << std::setw(11) << (running ? "RUNNING" : "STOPPED")
                      << std::setw(10) << (running ? std::to_string(p.getPid()) : "-")
                      << p.getRestarts()
                      << "\n";
        }
    }
}

void ProcessManager::logEvent(const std::string& type, const std::string& name, pid_t pid, int code) {
    std::cout << "[" << nowString() << "] "
              << type << " name=" << name << " pid=" << pid << " code=" << code << "\n";
}
