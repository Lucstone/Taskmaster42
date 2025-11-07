/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 02:06:35 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/07 06:06:29 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <atomic>
#include <cctype>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "../src/ProcessManager.hpp"   // on NE modifie pas ces fichiers
#include "../src/Helper.hpp"
#include "../src/Logger.hpp"
#include "../src/Utils.hpp"
#include "../src/ConfigManager.hpp"
#include "IPCServer.hpp"

// ============================= Globals / Signals ==============================
static std::atomic<bool> g_shouldExit(false);
static void onSigIntTerm(int) { g_shouldExit.store(true); }

// ========================== Client one-shot (réutilisable) ====================
static int runClientOnce(const std::string& sockPath, const std::string& commandLine) {
    int fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return 1; }

    sockaddr_un addr; std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", sockPath.c_str());

    if (::connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        ::close(fd);
        return 1;
    }

    std::string cmd = commandLine;
    if (cmd.empty() || cmd.back() != '\n') cmd.push_back('\n');
    if (::write(fd, cmd.c_str(), cmd.size()) < 0) {
        perror("write");
        ::close(fd);
        return 1;
    }

    std::string line; char ch; bool ok=false;
    while (true) {
        ssize_t n = ::read(fd, &ch, 1);
        if (n <= 0) break;
        if (ch == '\n') {
            if (line == ".") { ok = true; break; }
            std::cout << line << "\n";
            line.clear();
        } else {
            line.push_back(ch);
        }
    }
    ::close(fd);
    return ok ? 0 : 2;
}

// =========================== Readline: completion =============================
static const std::vector<std::string> COMMANDS = {
    "status","start","stop","restart","reload","quit","help"
};

static char* command_generator(const char* text, int state) {
    static size_t i, len;
    if (!state) { i = 0; len = std::strlen(text); }
    while (i < COMMANDS.size()) {
        const std::string& c = COMMANDS[i++];
        if (c.compare(0, len, text) == 0) return ::strdup(c.c_str());
    }
    return nullptr;
}

static char** completer(const char* text, int start, int end) {
    (void)start; (void)end;
    return rl_completion_matches(text, command_generator);
}

// =============================== Helpers locaux ==============================
static inline std::string trim(std::string s) {
    size_t a=0,b=s.size();
    while (a<b && std::isspace((unsigned char)s[a])) ++a;
    while (b>a && std::isspace((unsigned char)s[b-1])) --b;
    return s.substr(a,b-a);
}
static inline void toUpperInPlace(std::string& s) {
    for (char& c: s) c = std::toupper(static_cast<unsigned char>(c));
}

// =============================== MAIN ========================================
int main(int argc, char** argv) {
    const std::string sockPath = "/tmp/taskmaster.sock";

    // -------------------------- Mode CLIENT one-shot ---------------------------
    // ./taskmaster_bonus --client STATUS
    // ./taskmaster_bonus --client "START <name>"
    if (argc >= 2 && std::string(argv[1]) == "--client") {
        if (argc <= 2) {
            std::cerr << "usage: " << argv[0] << " --client <COMMAND...>\n";
            return 1;
        }
        std::string line;
        for (int i=2; i<argc; ++i) { if (i>2) line.push_back(' '); line += argv[i]; }
        return runClientOnce(sockPath, line);
    }

    // -------------------------- Mode SERVER (daemon) --------------------------
    // ./taskmaster_bonus --server [config.yaml]
    if (argc >= 2 && std::string(argv[1]) == "--server") {
        std::signal(SIGINT,  &onSigIntTerm);
        std::signal(SIGTERM, &onSigIntTerm);

        const std::string cfgPath = (argc >= 3) ? argv[2] : "config.yaml";

        ProcessManager pm;           // utilise ton parser via loadConfig
        pm.loadConfig(cfgPath);
        pm.startAutostartPrograms();

        std::cout << "[" << nowString() << "] Taskmaster server running on "
                  << sockPath << " (cfg=" << cfgPath << ")\n";

        IPCServer server(sockPath, [&](const std::string& raw) {
            std::vector<std::string> resp;

            std::string cmdline = trim(raw);
            const auto sp = cmdline.find(' ');
            std::string op  = (sp==std::string::npos? cmdline : cmdline.substr(0, sp));
            std::string arg = (sp==std::string::npos? ""      : trim(cmdline.substr(sp+1)));
            toUpperInPlace(op);

            if (op == "HELP") {
                resp.push_back("Commands: HELP, STATUS, START <name>, STOP <name>, RESTART <name>, RELOAD, QUIT");
            } else if (op == "STATUS") {
                // capture ce que pm.printStatus() écrit sur std::cout
                std::ostringstream oss;
                std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
                try { pm.printStatus(); } catch (...) { std::cout.rdbuf(old); throw; }
                std::cout.rdbuf(old);
                std::istringstream iss(oss.str());
                for (std::string l; std::getline(iss, l);) resp.push_back(l);
                Logger::log("STATUS", "-", getpid(), 0);
            } else if (op == "START") {
                if (arg.empty()) resp.push_back("ERR usage: START <name>");
                else {
                    try { pm.startProgram(arg); resp.push_back("OK start "+arg);
                          Logger::log("START", arg, getpid(), 0); }
                    catch (const std::exception& e){ Logger::log("ERR", "START "+arg, getpid(), 1);
                          resp.push_back(std::string("ERR ")+e.what()); }
                }
            } else if (op == "STOP") {
                if (arg.empty()) resp.push_back("ERR usage: STOP <name>");
                else {
                    try { pm.stopProgram(arg); resp.push_back("OK stop "+arg);
                          Logger::log("STOP", arg, getpid(), 0); }
                    catch (const std::exception& e){ Logger::log("ERR", "STOP "+arg, getpid(), 1);
                          resp.push_back(std::string("ERR ")+e.what()); }
                }
            } else if (op == "RESTART") {
                if (arg.empty()) resp.push_back("ERR usage: RESTART <name>");
                else {
                    try { pm.restartProgram(arg); resp.push_back("OK restart "+arg);
                          Logger::log("RESTART", arg, getpid(), 0); }
                    catch (const std::exception& e){ Logger::log("ERR", "RESTART "+arg, getpid(), 1);
                          resp.push_back(std::string("ERR ")+e.what()); }
                }
            } else if (op == "RELOAD") {
                try {
                    // validation via ConfigManager (sans toucher ProcessManager)
                    ConfigManager cm; cm.loadConfig(cfgPath);
                    pm.loadConfig(cfgPath);
                    pm.startAutostartPrograms();
                    Logger::log("RELOAD", cfgPath, getpid(), 0);
                    resp.push_back("OK reload");
                } catch (const std::exception& e) {
                    Logger::log("ERR", "RELOAD "+cfgPath, getpid(), 1);
                    resp.push_back(std::string("ERR ")+e.what());
                }
            } else if (op == "QUIT") {
                g_shouldExit.store(true);
                Logger::log("QUIT", "-", getpid(), 0);
                resp.push_back("OK quitting");
            } else {
                resp.push_back("ERR unknown command");
            }
            return resp;
        });

        while (!g_shouldExit.load()) {
            pm.tick();
            server.pollOnce();
            usleep(2000);
        }
        std::cout << "[" << nowString() << "] Taskmaster server stopped\n";
        return 0;
    }

    // ---------------------- Mode SHELL interactif (client) --------------------
    std::signal(SIGINT,  &onSigIntTerm);
    std::signal(SIGTERM, &onSigIntTerm);

    rl_catch_signals = 0;
    rl_attempted_completion_function = completer;

    std::cout << "taskmasterctl connected to " << sockPath
              << "\nType 'help' for commands, 'exit' to quit.\n";

    Helper help; // on réutilise ton Helper pour l'aide locale

    while (!g_shouldExit.load()) {
        char* input = readline("taskmasterclt> ");
        if (!input) break;
        std::string line(input);
        free(input);

        line = trim(line);
        if (line.empty()) continue;
        add_history(line.c_str());

        // aide locale via Helper (pas besoin de parler au daemon pour l'aide)
        if (line == "help" || line == "HELP") {
            std::vector<std::string> args; args.push_back("help");
            help.handle(args);
            continue;
        }
        if (line == "exit" || line == "quit") break;

        int rc = runClientOnce(sockPath, line);
        if (rc == 1) {
            std::cerr << "daemon non joignable. Lance d'abord:\n"
                         "  ./taskmaster_bonus --server [config.yaml]\n";
        }
    }

    std::cout << "Exiting Taskmaster shell.\n";
    return 0;
}
