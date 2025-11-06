/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 06:36:30 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/05 06:45:45 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <vector>
#include <readline/readline.h>
#include <readline/history.h>

#include "Help.hpp"
#include "ProcessManager.hpp"

// === Liste des commandes supportées ===
const std::vector<std::string> COMMANDS = {
    "status", "start", "stop", "restart", "reload", "quit"
};

// === Fonction d'autocomplétion ===
char* command_generator(const char* text, int state) {
    static size_t index;
    static size_t len;
    if (!state) {
        index = 0;
        len = strlen(text);
    }
    while (index < COMMANDS.size()) {
        const std::string& cmd = COMMANDS[index++];
        if (cmd.compare(0, len, text) == 0) {
            return strdup(cmd.c_str());
        }
    }
    return nullptr;
}

// Placeholder pour la complétion

char** completer(const char* text, int start, int end) {
    (void)start;
    (void)end;
    return rl_completion_matches(text, command_generator);
}


// Split simple par espaces
std::vector<std::string> split(const std::string& line) {
    std::vector<std::string> args;
    std::string cur;
    for (char c : line) {
        if (std::isspace(c)) {
            if (!cur.empty()) { args.push_back(cur); cur.clear(); }
        } else {
            cur.push_back(c);
        }
    }
    if (!cur.empty()) args.push_back(cur);
    return args;
}

// Handlers
void handle_status(ProcessManager& pm) {
    pm.printStatus();
}

void handle_start(ProcessManager& pm, const std::string& name) {
    pm.startProgram(name);
}

void handle_stop(ProcessManager& pm, const std::string& name) {
    pm.stopProgram(name);
}

void handle_restart(ProcessManager& pm, const std::string& name) {
    pm.restartProgram(name);
}

void handle_reload(ProcessManager& pm) {
    std::cout << "Reloading configuration...\n";
    pm.loadConfig("config.yaml");
    pm.startAutostartPrograms();
}

// ====================== MAIN =========================
int main() {
    rl_catch_signals = 0;
    rl_attempted_completion_function = completer;

    Help helpManager;
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
            else if (cmd == "start" && args.size() > 1) handle_start(pm, args[1]);
            else if (cmd == "stop" && args.size() > 1) handle_stop(pm, args[1]);
            else if (cmd == "restart" && args.size() > 1) handle_restart(pm, args[1]);
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