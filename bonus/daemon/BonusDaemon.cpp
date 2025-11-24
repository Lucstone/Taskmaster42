/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BonusDaemon.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 07:24:25 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/24 09:19:53 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BonusDaemon.hpp"

#include <sstream>
#include <iostream>
#include <functional>
#include <unistd.h>

BonusDaemon::BonusDaemon(const std::string& config_file,
                         const std::string& socket_path)
    : _config_file(config_file),
      _socket_path(socket_path),
      _config_parser(config_file),
      _process_manager(),
      _command_handler(&_process_manager),
      _server(socket_path,
              std::bind(&BonusDaemon::handleCommand_, this, std::placeholders::_1)),
      _running(true)
{
}

BonusDaemon::BonusDaemon(const BonusDaemon& other)
    : _config_file(other._config_file),
      _socket_path(other._socket_path),
      _config_parser(other._config_file),
      _process_manager(),
      _command_handler(&_process_manager),
      _server(other._socket_path,
              std::bind(&BonusDaemon::handleCommand_, this, std::placeholders::_1)),
      _running(other._running)
{
}

BonusDaemon& BonusDaemon::operator=(const BonusDaemon& other)
{
    if (this != &other) {
        _config_file   = other._config_file;
        _socket_path   = other._socket_path;
        _config_parser = ConfigParser(other._config_file);
        _process_manager = ProcessManager();
        _command_handler = CommandHandler(&_process_manager);
        _server = IPCServer(_socket_path,
                            std::bind(&BonusDaemon::handleCommand_, this, std::placeholders::_1));
        _running = other._running;
    }
    return *this;
}

BonusDaemon::~BonusDaemon() {}

void BonusDaemon::loadConfiguration()
{
    std::map<std::string, ProgramConfig> cfg =
        _config_parser.parse(_config_file);
    _process_manager.loadConfig(cfg);
}

void BonusDaemon::reloadConfiguration()
{
    try {
        std::map<std::string, ProgramConfig> cfg =
            _config_parser.parse(_config_file);
        _process_manager.reloadConfig(cfg);
        std::cout << "taskmaster_bonus: configuration reloaded\n";
    } catch (...) {
        std::cerr << "taskmaster_bonus: ERROR reloading configuration\n";
    }
}

void BonusDaemon::shutdown()
{
    std::cout << "taskmaster_bonus: shutting down...\n";
    _running = false;
}

std::vector<std::string>
BonusDaemon::handleCommand_(const std::string& line)
{
    std::stringstream buffer;

    std::streambuf* oldCout = std::cout.rdbuf(buffer.rdbuf());
    std::streambuf* oldCerr = std::cerr.rdbuf(buffer.rdbuf());

    _command_handler.execute(line);

    std::cout.rdbuf(oldCout);
    std::cerr.rdbuf(oldCerr);

    if (_command_handler.needsReload()) {
        _command_handler.clearReload();
        reloadConfiguration();
    }

    if (_command_handler.needsShutdown()) {
        _command_handler.clearShutdown();
        shutdown();
    }

    std::vector<std::string> out;
    std::string tmp;
    while (std::getline(buffer, tmp))
        out.push_back(tmp);

    if (out.empty())
        out.push_back("");

    return out;
}

void BonusDaemon::run()
{
    SignalHandler::setup();

    loadConfiguration();
    _process_manager.startAutostart();

    // 🔥 Correction critique : synchro immédiate
    _process_manager.handleSigchld();
    _process_manager.update();

    while (_running) {

        if (SignalHandler::needsProcessCheck()) {
            _process_manager.handleSigchld();
            SignalHandler::clearProcessCheck();
        }

        if (SignalHandler::needsConfigReload()) {
            SignalHandler::clearConfigReload();
            reloadConfiguration();
        }

        if (SignalHandler::shouldShutdown()) {
            SignalHandler::clearShutdown();
            shutdown();
        }

        _process_manager.update();
        _server.pollOnce();

        usleep(10000);
    }

    _process_manager.shutdown();
    std::cout << "taskmaster_bonus: stopped\n";
}
