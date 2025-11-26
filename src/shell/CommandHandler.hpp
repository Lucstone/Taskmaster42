#pragma once

#include "../process/ProcessManager.hpp"
#include <string>
#include <vector>

class CommandHandler {
private:
    ProcessManager  *_process_mgr;
    bool            _reload_requested;
    bool            _shutdown_requested;

    std::vector<std::string>    splitCommand(const std::string &cmd_line) const;

    void                        startSingleProcess(const std::string &name);
    void                        stopSingleProcess(const std::string &name);
    void                        restartSingleProcess(const std::string &name);
    void                        printProcessStatus(const ProcessStatus &status);

public:
    CommandHandler();
    CommandHandler(ProcessManager *mgr);
    CommandHandler(const CommandHandler &other);
    CommandHandler &operator=(const CommandHandler &other);
    ~CommandHandler();

    void                        status(const std::vector<std::string> &args);
    void                        start(const std::vector<std::string> &args);
    void                        stop(const std::vector<std::string> &args);
    void                        restart(const std::vector<std::string> &args);
    void                        reload(const std::vector<std::string> &args);
    void                        shutdown(const std::vector<std::string> &args);
    void                        help(const std::vector<std::string> &args);

    bool                        execute(const std::string &cmd_line);

    std::vector<std::string>    getProgramNames() const;

    bool                        needsReload() const { return _reload_requested; }
    bool                        needsShutdown() const { return _shutdown_requested; }
    void                        clearReload() { _reload_requested = false; }
    void                        clearShutdown() { _shutdown_requested = false; }
};
