/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_bonus.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/07 02:06:35 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/26 04:25:05 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "../src/utils/Utils.hpp"
#include "../src/logger/Logger.hpp"
#include "daemon/BonusDaemon.hpp"
#include "shell/BonusShell.hpp"
#include "client/BonusClient.hpp"

static const char* DEFAULT_SOCKET_PATH = "/tmp/taskmaster.sock";

static std::string joinArgs_(int argc, char* argv[], int start) {
    std::string res;
    for (int i = start; i < argc; ++i) {
        if (i > start)
            res += ' ';
        res += argv[i];
    }
    return res;
}

int main(int argc, char* argv[]) 
{
    bool        serverMode = false;
    std::string configFile;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--server") {
            serverMode = true;
        } 
        else if (configFile.empty()) {
            configFile = arg;
        } 
        else {
             std::cout << "Taskmaster bonus client. Wrong Argument.\n";
        }
    }

    if (configFile.empty())
        configFile = "config/taskmaster.yaml";

    if (serverMode) 
    {
        try {
            if (!Utils::fileExists("logs"))
                Utils::createDirectory("logs");
            Logger::getInstance("logs/taskmaster_bonus.log")->setMinLevel(LogLevel::DEBUG);
            LOG_INFO("Taskmaster BONUS daemon starting");
            BonusDaemon daemon(configFile, DEFAULT_SOCKET_PATH);
            daemon.run();
            Logger::destroyInstance();
            return 0;
        } 
        catch (const std::exception& e) {
            std::cerr << "taskmaster_bonus: ERROR - wrong yaml"
                      << e.what() << "\n";
            LOG_ERROR(std::string("Error yaml (bonus): ") + e.what());
            Logger::destroyInstance();
            return 1;
        }
    }
    if (argc > 1) {
        std::string cmd = joinArgs_(argc, argv, 1);
        BonusClient client;
        return client.sendCommand(DEFAULT_SOCKET_PATH, cmd);
    }

    BonusShell shell;
    return shell.run(DEFAULT_SOCKET_PATH);
}
