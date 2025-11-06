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

#include "ProcessManager.hpp"
#include "Helper.hpp"

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

int main() {
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
