#include "CommandHandler.hpp"
#include "../signal/SignalHandler.hpp"
#include "../utils/Utils.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

CommandHandler::CommandHandler()
    : _process_mgr(NULL), _reload_requested(false), _shutdown_requested(false) {
}

CommandHandler::CommandHandler(ProcessManager *mgr)
    : _process_mgr(mgr), _reload_requested(false), _shutdown_requested(false) {
}

CommandHandler::CommandHandler(const CommandHandler &other)
    : _process_mgr(other._process_mgr),
      _reload_requested(other._reload_requested),
      _shutdown_requested(other._shutdown_requested) {
}

CommandHandler &CommandHandler::operator=(const CommandHandler &other) {
    if (this != &other) {
        _process_mgr = other._process_mgr;
        _reload_requested = other._reload_requested;
        _shutdown_requested = other._shutdown_requested;
    }
    return *this;
}

CommandHandler::~CommandHandler() {
}

bool CommandHandler::execute(const std::string &cmd_line) {
    if (_process_mgr) {
        if (SignalHandler::needsProcessCheck()) {
            _process_mgr->handleSigchld();
            SignalHandler::clearProcessCheck();
        }

        _process_mgr->update();
    }

    std::vector<std::string>    args = splitCommand(cmd_line);

    if (args.empty()) return true;

    std::string command = args[0];

    if (command == "status") {
        status(args);
    } else if (command == "start") {
        start(args);
    } else if (command == "stop") {
        stop(args);
    } else if (command == "restart") {
        restart(args);
    } else if (command == "reload") {
        reload(args);
    } else if (command == "quit") {
        shutdown(args);
    } else if (command == "help") {
        help(args);
    } else {
        std::cout << "*** Unknown command: " << command << "\n";
    }

    return true;
}

void CommandHandler::status(const std::vector<std::string> &args) {
    if (args.size() == 1) {
        std::vector<ProcessStatus>  statuses = _process_mgr->getAllStatus();

        for (size_t i = 0; i < statuses.size(); ++i) {
            printProcessStatus(statuses[i]);
        }
    } else {
        for (size_t i = 1; i < args.size(); ++i) {
            std::string name = args[i];

            if (name == "all") {
                std::vector<ProcessStatus>  statuses = _process_mgr->getAllStatus();

                for (size_t j = 0; j < statuses.size(); ++j) {
                    printProcessStatus(statuses[j]);
                }
            } else {
                ProcessStatus   st = _process_mgr->getProcessStatus(name);

                if (!st.exists) {
                    std::cout << name << ": ERROR (no such process)\n";
                } else {
                    printProcessStatus(st);
                }
            }
        }
    }
}

void CommandHandler::printProcessStatus(const ProcessStatus &status) {
    std::ostringstream  oss;

    oss << status.name;

    if (status.name.length() < 32) {
        oss << std::string(33 - status.name.length(), ' ');
    } else {
        oss << "   ";
    }

    oss << status.state_str;

    if (!status.info.empty()) {
        oss << "   " << status.info;
    }

    std::cout << oss.str() << "\n";
}

void CommandHandler::start(const std::vector<std::string> &args) {
    if (args.size() < 2) {
        std::cout << "Error: start requires a process name\n";
        return;
    }

    for (size_t i = 1; i < args.size(); ++i) {
        std::string name = args[i];

        if (name == "all") {
            std::vector<std::string>    all_programs = _process_mgr->getProgramNames();

            for (size_t j = 0; j < all_programs.size(); ++j) {
                startSingleProcess(all_programs[j]);
            }
        } else {
            startSingleProcess(name);
        }
    }
}

void CommandHandler::startSingleProcess(const std::string &name) {
    StartResult result = _process_mgr->startProgram(name);

    switch (result) {
        case START_SUCCESS:
            std::cout << name << ": started\n";
            break;
        case START_ALREADY_STARTED:
            std::cout << name << ": ERROR (already started)\n";
            break;
        case START_NO_SUCH_PROCESS:
            std::cout << name << ": ERROR (no such process)\n";
            break;
        case START_SPAWN_ERROR:
            std::cout << name << ": ERROR (spawn error)\n";
            break;
        case START_NO_SUCH_FILE:
            std::cout << name << ": ERROR (no such file)\n";
            break;
    }
}

void CommandHandler::stop(const std::vector<std::string> &args) {
    if (args.size() < 2) {
        std::cout << "Error: stop requires a process name\n";
        return;
    }

    for (size_t i = 1; i < args.size(); ++i) {
        std::string name = args[i];

        if (name == "all") {
            std::vector<std::string>    all_programs = _process_mgr->getProgramNames();

            for (size_t j = 0; j < all_programs.size(); ++j) {
                stopSingleProcess(all_programs[j]);
            }
        } else {
            stopSingleProcess(name);
        }
    }
}

void CommandHandler::stopSingleProcess(const std::string &name) {
    StopResult  result = _process_mgr->stopProgram(name);

    switch (result) {
        case STOP_SUCCESS:
            std::cout << name << ": stopped\n";
            break;
        case STOP_NOT_RUNNING:
            std::cout << name << ": ERROR (not running)\n";
            break;
        case STOP_NO_SUCH_PROCESS:
            std::cout << name << ": ERROR (no such process)\n";
            break;
    }
}

void CommandHandler::restart(const std::vector<std::string> &args) {
    if (args.size() < 2) {
        std::cout << "Error: restart requires a process name\n";
        return;
    }

    for (size_t i = 1; i < args.size(); ++i) {
        std::string name = args[i];

        if (name == "all") {
            std::vector<std::string>    all_programs = _process_mgr->getProgramNames();

            for (size_t j = 0; j < all_programs.size(); ++j) {
                restartSingleProcess(all_programs[j]);
            }
        } else {
            restartSingleProcess(name);
        }
    }
}

void CommandHandler::restartSingleProcess(const std::string &name) {
    RestartResult   result = _process_mgr->restartProgram(name);

    switch (result) {
        case RESTART_SUCCESS:
            break;
        case RESTART_NOT_RUNNING_STARTED:
            std::cout << name << ": not running\n";
            std::cout << name << ": started\n";
            break;
        case RESTART_NO_SUCH_PROCESS:
            std::cout << name << ": ERROR (no such process)\n";
            break;
        case RESTART_SPAWN_ERROR:
            std::cout << name << ": ERROR (spawn error)\n";
            break;
        case RESTART_NO_SUCH_FILE:
            std::cout << name << ": ERROR (no such file)\n";
            break;
    }
}

void CommandHandler::reload(const std::vector<std::string> &args) {
    if (args.size() > 1) {
        std::cout << "Error: reload accepts no arguments\n";
        return;
    }

    std::cout << "Really restart the remote supervisord process y/N? ";
    std::string response;
    std::getline(std::cin, response);

    response = Utils::trim(response);
    response = Utils::toLower(response);

    if (response == "y" || response == "yes") {
        _reload_requested = true;
        std::cout << "Restarted supervisord\n";
    }
}

void CommandHandler::shutdown(const std::vector<std::string> &args) {
    (void)args;
    _shutdown_requested = true;
}

void CommandHandler::help(const std::vector<std::string> &args) {
    if (args.size() == 1) {
        std::cout << "\n";
        std::cout << "default commands (type help <topic>):\n";
        std::cout << "=====================================\n";
        std::cout << "help     reload   restart   start    \n";
        std::cout << "status   stop     quit               \n";
        std::cout << "\n";
    } else {
        std::string topic = args[1];

        if (topic == "start") {
            std::cout << "start <name>            Start a process\n";
            std::cout << "start <name> <name>     Start multiple processes or groups\n";
            std::cout << "start all               Start all processes\n";
        } else if (topic == "stop") {
            std::cout << "stop <name>             Stop a process\n";
            std::cout << "stop <name> <name>      Stop multiple processes or groups\n";
            std::cout << "stop all                Stop all processes\n";
        } else if (topic == "restart") {
            std::cout << "restart <name>          Restart a process\n";
            std::cout << "restart <name> <name>   Restart multiple processes or groups\n";
            std::cout << "restart all             Restart all processes\n";
            std::cout << "Note: restart does not reread config files. For that, see reread and update.\n";
        } else if (topic == "status") {
            std::cout << "status <name>           Get status for a single process\n";
            std::cout << "status <name> <name>    Get status for multiple named processes\n";
            std::cout << "status                  Get all process status info\n";
        } else if (topic == "reload") {
            std::cout << "reload          Restart the remote supervisord.\n";
        } else if (topic == "quit") {
            std::cout << "quit    Exit the supervisor shell.\n";
        } else if (topic == "help") {
            std::cout << "help            Show available commands\n";
            std::cout << "help <command>  Show help for a specific command\n";
        } else {
            std::cout << "*** No help on " << topic << "\n";
        }
    }
}

std::vector<std::string> CommandHandler::splitCommand(const std::string &cmd_line) const {
    return Utils::split(Utils::trim(cmd_line), ' ');
}

std::vector<std::string> CommandHandler::getProgramNames() const {
    return _process_mgr->getProgramNames();
}
