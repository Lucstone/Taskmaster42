#include "../include/ProgramConfig.hpp"
#include <iostream>
#include <iomanip>   // std::setw
#include <map>
#include <vector>

static void printOne(const std::string& name, const ProgramConfig& cfg) {
    std::cout << "============================= [" << name << "] =============================\n";
    std::cout << "name        : " << cfg.getName() << std::endl;
    std::cout << "cmd         : " << cfg.getCmd() << std::endl;
    std::cout << "numprocs    : " << cfg.getNumprocs() << std::endl;
    std::cout << "autostart   : " << (cfg.getAutostart() ? "true" : "false") << std::endl;
    std::cout << "autorestart : " << cfg.getAutorestart() << std::endl;
    std::cout << "exitcodes   : ";
    for (size_t i = 0; i < cfg.getExitcodes().size(); ++i) {
        std::cout << cfg.getExitcodes()[i] << (i + 1 < cfg.getExitcodes().size() ? " " : "");
    }
    std::cout << std::endl;
    std::cout << "startretries: " << cfg.getStartretries() << std::endl;
    std::cout << "starttime   : " << cfg.getStarttime() << std::endl;
    std::cout << "stopsignal  : " << cfg.getStopsignals() << std::endl;
    std::cout << "stoptime    : " << cfg.getStoptime() << std::endl;
    std::cout << "workingdir  : " << cfg.getWorkingDir() << std::endl;
    std::cout << "umask       : " << cfg.getUmask() << std::endl;
    std::cout << "stdout      : " << cfg.getStdoutPath() << std::endl;
    std::cout << "stderr      : " << cfg.getStderrPath() << std::endl;

    if (!cfg.getEnv().empty()) {
        std::cout << "env:\n";
        for (const auto& kv : cfg.getEnv()) {
            std::cout << "  " << std::left << std::setw(16) << kv.first << " = " << kv.second << std::endl;
        }
    } else {
        std::cout << "env         : (empty)\n";
    }
    std::cout << std::endl;
}

int main(int argc, char** argv) {
    const char* path = (argc > 1) ? argv[1] : "config.yaml";
    try {
        // Charge tous les programmes définis dans config.yaml
        std::map<std::string, ProgramConfig> programs = ProgramConfig::loadAll(path);

        std::cout << "Loaded " << programs.size() << " program(s) from '" << path << "'\n\n";
        for (const auto& it : programs) {
            // it.first = nom du programme (clé YAML)
            // it.second = ProgramConfig rempli
            printOne(it.first, it.second);
        }

        // Petit sanity-check optionnel : vérifier qu'au moins cmd est non vide
        for (const auto& it : programs) {
            if (it.second.getCmd().empty()) {
                std::cerr << "[ERROR] Program '" << it.first << "' has empty cmd()\n";
                return 2;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "[Parse error] " << e.what() << std::endl;
        return 1;
    }
    return 0;
}