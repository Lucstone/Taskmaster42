#include "Help.hpp"
#include <iostream>
#include <sstream>

Help::Help() {
    commands = {"status", "start", "stop", "restart", "reload", "quit", "help"};

    helpTexts = {
        {"start",
            "start <name>\t\tStart a process\n"
            "start <name> <name>\tStart multiple processes or groups\n"
            "start all\t\tStart all processes\n"},
        {"restart",
            "restart <name>\t\tRestart a process\n"
            "restart <name> <name>\tRestart multiple processes or groups\n"
            "restart all\t\tRestart all processes\n"},
        {"status",
            "status <name>\t\tGet status for a single process\n"
            "status <name> <name>\tGet status for multiple named processes\n"
            "status\t\t\tGet all process status info\n"},
        {"stop",
            "stop <name>\t\tStop a process\n"
            "stop <name> <name>\tStop multiple processes or groups\n"
            "stop all\t\tStop all processes\n"},
        {"reload",
            "reload\t\t\tRestart the remote supervisord.\n"},
        {"quit",
            "quit\t\t\tExit the supervisor shell.\n"},
        {"help",
            "help [command]\t\tShow help for a command or list all commands.\n"}
    };
}

bool Help::is_known_command(const std::string& cmd) const {
    for (const auto& c : commands)
        if (c == cmd)
            return true;
    return false;
}

void Help::print_default_help() const {
    std::cout << "\ndefault commands (type help <topic>):\n";
    std::cout << "=====================================\n";

    int count = 0;
    for (const auto& cmd : commands) {
        std::cout << cmd;
        count++;
        if (count % 6 == 0 || cmd == commands.back())
            std::cout << "\n";
        else
            std::cout << "   ";
    }
    std::cout << "\n";
}

void Help::print_command_help(const std::vector<std::string>& args) const {
    if (args.size() == 2) {
        const std::string& cmd = args[1];
        auto it = helpTexts.find(cmd);
        if (it != helpTexts.end()) {
            std::cout << it->second;
        } else {
            std::cout << "*** No help on " << cmd << "\n";
        }
    } else {
        // Combine les arguments restants (pour afficher les erreurs)
        std::ostringstream oss;
        for (size_t i = 1; i < args.size(); ++i) {
            if (i > 1) oss << " ";
            oss << args[i];
        }
        std::cout << "*** No help on " << oss.str() << "\n";
    }
}

void Help::handle(const std::vector<std::string>& args) const {
    if (args.size() == 1)
        print_default_help();
    else
        print_command_help(args);
}