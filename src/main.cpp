/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 06:36:30 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/06 09:18:21 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <vector>
#include <readline/readline.h>
#include <readline/history.h>

#include "ProcessManager.hpp"
#include "Helper.hpp"
#include "IPCServer.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <csignal>

static std::atomic<bool> g_shouldExit(false);
static void onSigIntTerm(int) { g_shouldExit.store(true); }
// Client ultra-léger: envoie UNE commande au daemon et affiche la réponse.
// Réponse multi-lignes terminée par une ligne ".".
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

    std::string line; char ch; bool ok = false;
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

const std::vector<std::string> COMMANDS = {
    "status", "start", "stop", "restart", "reload", "quit", "help"
};

char* command_generator(const char* text, int state) {
    static size_t index, len;
    if (!state) { index = 0; len = strlen(text); }
    while (index < COMMANDS.size()) {
        const std::string& cmd = COMMANDS[index++];
        if (cmd.compare(0, len, text) == 0) return strdup(cmd.c_str());
    }
    return nullptr;
}

char** completer(const char* text, int start, int end) {
    (void)start; (void)end;
    return rl_completion_matches(text, command_generator);
}

std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> args;
    std::string cur;
    for (char c : line) {
        if (isspace(c)) { if (!cur.empty()) { args.push_back(cur); cur.clear(); } }
        else cur.push_back(c);
    }
    if (!cur.empty()) args.push_back(cur);
    return args;
}

void handle_status(ProcessManager& pm) {
    pm.printStatus();
}

void handle_start(ProcessManager& pm, const std::vector<std::string>& args) {
    for (size_t i = 1; i < args.size(); ++i)
        pm.startProgram(args[i]);
}

void handle_stop(ProcessManager& pm, const std::vector<std::string>& args) {
    for (size_t i = 1; i < args.size(); ++i)
        pm.stopProgram(args[i]);
}

void handle_restart(ProcessManager& pm, const std::vector<std::string>& args) {
    for (size_t i = 1; i < args.size(); ++i)
        pm.restartProgram(args[i]);
}

void handle_reload(ProcessManager& pm) {
    std::cout << "Reloading configuration...\n";
    pm.loadConfig("config.yaml");
    pm.startAutostartPrograms();
}

/*int main() {
    rl_catch_signals = 0;
    rl_attempted_completion_function = completer;

    Helper helpManager;
    ProcessManager pm;

    pm.loadConfig("config.yaml");
    pm.startAutostartPrograms();

    while (true) {
        char* input = readline("taskmaster> ");
        if (!input) break;

        std::string line(input);
        free(input);

        if (line.empty()) continue;
        add_history(line.c_str());

        auto args = split(line);
        const std::string& cmd = args[0];

        try {
            if (cmd == "status") handle_status(pm);
            else if (cmd == "start") handle_start(pm, args);
            else if (cmd == "stop") handle_stop(pm, args);
            else if (cmd == "restart") handle_restart(pm, args);
            else if (cmd == "reload") handle_reload(pm);
            else if (cmd == "quit") break;
            else if (cmd == "help") helpManager.handle(args);
            else std::cout << "*** Unknown syntax: " << cmd << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    std::cout << "Exiting Taskmaster shell.\n";
    return 0;
}*/


// suppose que tu as déjà ta fonction split(string)
extern std::vector<std::string> split(const std::string& line);
extern char** completer(const char* text, int start, int end);
extern void handle_status(ProcessManager&);
extern void handle_start(ProcessManager&, const std::vector<std::string>&);
extern void handle_stop(ProcessManager&, const std::vector<std::string>&);
extern void handle_restart(ProcessManager&, const std::vector<std::string>&);
extern void handle_reload(ProcessManager&);

// =============================== MAIN ========================================
int main(int argc, char** argv) {
    const std::string sockPath = "/tmp/taskmaster.sock";

    // Mode CLIENT: ./taskmaster --client STATUS
    if (argc >= 2 && std::string(argv[1]) == "--client") {
        if (argc <= 2) {
            std::cerr << "usage: " << argv[0] << " --client <COMMAND...>\n";
            return 1;
        }
        std::string line;
        for (int i = 2; i < argc; ++i) { if (i > 2) line.push_back(' '); line += argv[i]; }
        return runClientOnce(sockPath, line);
    }

    // Mode SERVER: ./taskmaster --server
    if (argc >= 2 && std::string(argv[1]) == "--server") {
        std::signal(SIGINT, &onSigIntTerm);
        ProcessManager pm;
        pm.loadConfig("config.yaml");
        pm.startAutostartPrograms();

        IPCServer server(sockPath, [&](const std::string& raw) {
            auto trim = [](std::string s){
                while(!s.empty() && isspace((unsigned char)s.back())) s.pop_back();
                size_t i=0; while(i<s.size() && isspace((unsigned char)s[i])) ++i;
                return s.substr(i);
            };
            std::vector<std::string> resp;
            std::string cmdline = trim(raw);
            auto sp = cmdline.find(' ');
            std::string op  = (sp == std::string::npos ? cmdline : cmdline.substr(0, sp));
            std::string arg = (sp == std::string::npos ? ""       : trim(cmdline.substr(sp+1)));

            if (op == "HELP" || op == "help") {
                resp.push_back("Commands: HELP, STATUS, START <name>, STOP <name>, RESTART <name>, RELOAD, QUIT");
            } else if (op == "STATUS") {
                std::ostringstream oss;
                pm.printStatus(); // adapte si besoin
                resp.push_back("STATUS printed on server");
            } else if (op == "START") {
                if (arg.empty()) resp.push_back("ERR usage: START <name>");
                else { try { pm.startProgram(arg); resp.push_back("OK start " + arg); }
                       catch (const std::exception& e){ resp.push_back(std::string("ERR ") + e.what()); } }
            } else if (op == "STOP") {
                if (arg.empty()) resp.push_back("ERR usage: STOP <name>");
                else { try { pm.stopProgram(arg); resp.push_back("OK stop " + arg); }
                       catch (const std::exception& e){ resp.push_back(std::string("ERR ") + e.what()); } }
            } else if (op == "RESTART") {
                if (arg.empty()) resp.push_back("ERR usage: RESTART <name>");
                else { try { pm.restartProgram(arg); resp.push_back("OK restart " + arg); }
                       catch (const std::exception& e){ resp.push_back(std::string("ERR ") + e.what()); } }
            } else if (op == "RELOAD") {
                try { pm.loadConfig("config.yaml"); pm.startAutostartPrograms(); resp.push_back("OK reload"); }
                catch (const std::exception& e){ resp.push_back(std::string("ERR ") + e.what()); }
            } else if (op == "QUIT") {
                g_shouldExit.store(true);
                resp.push_back("OK quitting");
            } else {
                resp.push_back("ERR unknown command");
            }
            return resp;
        });

        std::cout << "Taskmaster server running on " << sockPath
                  << " (use: ./taskmaster --client STATUS)\n";
        while (!g_shouldExit.load()) {
            pm.tick();
            server.pollOnce();
            usleep(2000);
        }
        return 0;
    }

    // ======================== Mode SHELL interactif ============================
    rl_catch_signals = 0;
    rl_attempted_completion_function = completer;

    Helper helpManager;
    ProcessManager pm;
    pm.loadConfig("config.yaml");
    pm.startAutostartPrograms();

    while (true) {
        char* input = readline("taskmaster> ");
        if (!input) break;

        std::string line(input);
        free(input);

        if (line.empty()) continue;
        add_history(line.c_str());

        auto args = split(line);
        const std::string& cmd = args[0];

        try {
            if (cmd == "status") handle_status(pm);
            else if (cmd == "start") handle_start(pm, args);
            else if (cmd == "stop") handle_stop(pm, args);
            else if (cmd == "restart") handle_restart(pm, args);
            else if (cmd == "reload") handle_reload(pm);
            else if (cmd == "quit") break;
            else if (cmd == "help") helpManager.handle(args);
            else std::cout << "*** Unknown syntax: " << cmd << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    std::cout << "Exiting Taskmaster shell.\n";
    return 0;
}
