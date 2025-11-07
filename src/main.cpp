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

int main() {
    Helper          hm;
    ProcessManager  pm;

    pm.loadConfig("config.yaml");
    pm.printStatus();

    rl_catch_signals = 0;
    rl_attempted_completion_function = completer;

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
            if (cmd == "quit") break;
            else if (cmd == "help") hm.handle(args);
            else if (cmd == "status") pm.handle_status(args);
            else if (cmd == "start") pm.handle_start(args);
            else if (cmd == "stop") pm.handle_stop(args);
            else if (cmd == "restart") pm.handle_restart(args);
            else if (cmd == "reload") pm.handle_reload();
            else std::cout << "*** Unknown syntax: " << cmd << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }

    return 0;
}
