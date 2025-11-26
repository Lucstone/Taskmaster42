#pragma once

#include "CommandHandler.hpp"
#include <string>
#include <vector>
#include <functional>

class Shell {
private:
    CommandHandler              *_handler;
    bool                        _running;
    std::vector<std::string>    _history;
    int                         _history_index;
    std::function<void()>       _onReload;

    void        setupReadline();
    std::string readCommand();
    void        addToHistory(const std::string &cmd);

public:
    Shell();
    Shell(CommandHandler *cmd_handler);
    Shell(const Shell &other);
    Shell &operator=(const Shell &other);
    ~Shell();

    void        run();
    void        stop();

    void        setReloadCallback(std::function<void()> cb) { _onReload = cb; }
    void        processCommand(const std::string &line);
    bool        checkInput(std::string &command);
};
