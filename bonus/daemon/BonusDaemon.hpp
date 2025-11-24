/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BonusDaemon.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 18:00:00 by YOU               #+#    #+#             */
/*   Updated: 2025/11/24 09:14:18 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

#include "../../src/config/ConfigParser.hpp"
#include "../../src/config/ProgramConfig.hpp"
#include "../../src/process/ProcessManager.hpp"
#include "../../src/shell/CommandHandler.hpp"
#include "../../src/signal/SignalHandler.hpp"
#include "../../src/logger/Logger.hpp"
#include "../../src/utils/Utils.hpp"

#include "../ipc/IPCServer.hpp"

class BonusDaemon {
public:
    BonusDaemon(const std::string& config_file,
                const std::string& socket_path);
    BonusDaemon(const BonusDaemon& other);
    BonusDaemon& operator=(const BonusDaemon& other);
    ~BonusDaemon();

    void run();

private:
    std::string     _config_file;
    std::string     _socket_path;

    ConfigParser    _config_parser;
    ProcessManager  _process_manager;
    CommandHandler  _command_handler;

    IPCServer       _server;
    bool            _running;

    void loadConfiguration();
    void reloadConfiguration();
    void shutdown();

    std::vector<std::string> handleCommand_(const std::string& line);
};

