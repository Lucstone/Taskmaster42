/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 02:06:35 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/24 07:38:08 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>

#include "../src/utils/Utils.hpp"
#include "../src/logger/Logger.hpp"

#include "daemon/BonusDaemon.hpp"
#include "shell/BonusShell.hpp"
#include "client/BonusClient.hpp"

static const char* DEFAULT_SOCKET_PATH = "/tmp/taskmaster.sock";

// Recolle argv[i] en une seule chaîne (ex: "start test")
static std::string joinArgs_(int argc, char* argv[], int start) {
    std::string res;
    for (int i = start; i < argc; ++i) {
        if (i > start)
            res += ' ';
        res += argv[i];
    }
    return res;
}

int main(int argc, char* argv[]) {
    bool        serverMode = false;
    std::string configFile;

    // -----------------------------------------------------------------
    // Parsing très simple des arguments pour le BONUS
    //
    //   ./taskmaster_bonus --server [config.yaml]
    //   ./taskmaster_bonus status
    //   ./taskmaster_bonus start test
    // -----------------------------------------------------------------
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--server") {
            serverMode = true;
        } else if (configFile.empty()) {
            // premier argument non "--server" = fichier de config
            configFile = arg;
        } else {
            // arguments en trop : on pourrait afficher un message,
            // mais pour l’instant on les ignore.
        }
    }

    if (configFile.empty())
        configFile = "config/taskmaster.yaml";

    // ==================== MODE DAEMON ====================
    if (serverMode) {
        try {
            if (!Utils::fileExists("logs"))
                Utils::createDirectory("logs");

            Logger::getInstance("logs/taskmaster_bonus.log")
                ->setMinLevel(LogLevel::DEBUG);

            LOG_INFO("=== Taskmaster BONUS daemon starting ===");

            BonusDaemon daemon(configFile, DEFAULT_SOCKET_PATH);
            daemon.run();

            Logger::destroyInstance();
            return 0;
        } catch (const std::exception& e) {
            std::cerr << "taskmaster_bonus: FATAL ERROR - "
                      << e.what() << "\n";
            LOG_ERROR(std::string("Fatal error (bonus): ") + e.what());
            Logger::destroyInstance();
            return 1;
        }
    }

    // ==================== MODE CLIENT ====================
    // - avec arguments : one-shot
    //   ex: ./taskmaster_bonus status
    // - sans arguments : shell interactif
    if (argc > 1) {
        std::string cmd = joinArgs_(argc, argv, 1);
        BonusClient client;
        return client.sendCommand(DEFAULT_SOCKET_PATH, cmd);
    }

    BonusShell shell;
    return shell.run(DEFAULT_SOCKET_PATH);
}
