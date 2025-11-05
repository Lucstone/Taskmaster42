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
#include <sstream>
#include <readline/readline.h>
#include <readline/history.h>
#include "Help.hpp"
#include "ProcessManager.hpp"

#include <unistd.h>

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

char** completer(const char* text, int start, int end) {
    (void)start;
    (void)end;
    return rl_completion_matches(text, command_generator);
}

// === Helpers ===
std::vector<std::string> split(const std::string& line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) tokens.push_back(token);
    return tokens;
}

void handle_status()   { std::cout << "[STATUS] Displaying all jobs...\n"; }
void handle_start(const std::string& prog)   { std::cout << "[START] Launching " << prog << "...\n"; }
void handle_stop(const std::string& prog)    { std::cout << "[STOP] Stopping " << prog << "...\n"; }
void handle_restart(const std::string& prog) { std::cout << "[RESTART] Restarting " << prog << "...\n"; }
void handle_reload()   { std::cout << "[RELOAD] Reloading configuration...\n"; }
void handle_quit()     { std::cout << "[QUIT] Shutting down Taskmaster.\n"; }

// === Main Loop ===
int main() {
    Help            helpManager;
    ProcessManager  pm;

    rl_catch_signals = 0;
    rl_attempted_completion_function = completer;
    
    pm.loadConfig("config.yaml");

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
            if (cmd == "status") handle_status();
            else if (cmd == "start" && args.size() > 1) handle_start(args[1]);
            else if (cmd == "stop" && args.size() > 1) handle_stop(args[1]);
            else if (cmd == "restart" && args.size() > 1) handle_restart(args[1]);
            else if (cmd == "reload") {
                std::cout << "Reloading configuration...\n";
                pm.loadConfig("config.yaml");
            }
            else if (cmd == "quit") { handle_quit(); break; }
            else if (cmd == "help") helpManager.handle(args);
            else {
                std::cout << "*** Unknown syntax: " << cmd << "\n";
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    return 0;
}