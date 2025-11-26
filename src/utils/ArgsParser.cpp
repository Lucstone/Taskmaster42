#include "ArgsParser.hpp"
#include "Utils.hpp"
#include <iostream>
#include <unistd.h>

std::string findDefaultConfigFile() {
    const char  *default_paths[] = {
        "taskmaster.yaml",
        "etc/taskmaster.yaml",
        "/etc/taskmaster.yaml",
        "/etc/taskmaster/taskmaster.yaml",
        nullptr
    };

    for (int i = 0; default_paths[i] != nullptr; ++i) {
        if (Utils::fileExists(default_paths[i])) {
            return std::string(default_paths[i]);
        }
    }

    return "";
}

void printUsage(const char *program_name) {
    std::cerr << "Usage: " << program_name << " [-c config_file]\n\n";
    std::cerr << "Options:\n";
    std::cerr << "  -c FILE    Specify configuration file\n\n";
    std::cerr << "If no config file is specified, taskmaster will search for:\n";
    std::cerr << "  1. taskmaster.yaml (current directory)\n";
    std::cerr << "  2. etc/taskmaster.yaml\n";
    std::cerr << "  3. /etc/taskmaster.yaml\n";
    std::cerr << "  4. /etc/taskmaster/taskmaster.yaml\n";
}

void printConfigNotFoundError() {
    std::cerr << "Error: No config file found at default paths "
              << "(taskmaster.yaml, etc/taskmaster.yaml, "
              << "/etc/taskmaster.yaml, /etc/taskmaster/taskmaster.yaml); "
              << "use the -c option to specify a config file at a different path\n";
}

std::string parseArguments(int argc, char *argv[]) {
    std::string config_file;
    int         opt;

    while ((opt = getopt(argc, argv, "c:h")) != -1) {
        switch (opt) {
            case 'c':
                config_file = optarg;
                break;
            case 'h':
                printUsage(argv[0]);
                exit(0);
            case '?':
            default:
                printUsage(argv[0]);
                exit(1);
        }
    }

    if (!config_file.empty()) {
        if (!Utils::fileExists(config_file)) {
            std::cerr << "Error: Config file not found: " << config_file << "\n";
            return "";
        }
        return config_file;
    }

    config_file = findDefaultConfigFile();

    if (config_file.empty()) {
        printConfigNotFoundError();
        return "";
    }

    return config_file;
}
