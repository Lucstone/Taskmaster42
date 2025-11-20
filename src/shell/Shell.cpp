#include "Shell.hpp"
#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/select.h>
#include <unistd.h>

// Global pointer for readline completion (needed for C callback)
static CommandHandler* g_command_handler = NULL;

// Completion generator for program names
char* program_name_generator(const char* text, int state) {
    static std::vector<std::string> matches;
    static size_t match_index;
    
    if (state == 0) {
        // First call - initialize matches
        matches.clear();
        match_index = 0;
        
        if (g_command_handler) {
            std::vector<std::string> programs = g_command_handler->getProgramNames();
            std::string prefix(text);
            
            for (size_t i = 0; i < programs.size(); ++i) {
                if (programs[i].find(prefix) == 0) {
                    matches.push_back(programs[i]);
                }
            }
            
            // Add "all" keyword
            if (std::string("all").find(prefix) == 0) {
                matches.push_back("all");
            }
        }
    }
    
    if (match_index < matches.size()) {
        return strdup(matches[match_index++].c_str());
    }
    
    return NULL;
}

// Completion generator for commands
char* command_name_generator(const char* text, int state) {
    static const char* commands[] = {
        "help", "status", "start", "stop", "restart", "reload", "quit", NULL
    };
    static int index;
    
    if (state == 0) {
        index = 0;
    }
    
    while (commands[index]) {
        const char* cmd = commands[index++];
        if (strncmp(cmd, text, strlen(text)) == 0) {
            return strdup(cmd);
        }
    }
    
    return NULL;
}

// Main completion function
char** shell_completion(const char* text, int start, int end) {
    (void)end;
    char** matches = NULL;
    
    // If at start of line, complete commands
    if (start == 0) {
        matches = rl_completion_matches(text, command_name_generator);
    } else {
        // Otherwise, complete program names
        // Get the command (first word)
        std::string line(rl_line_buffer);
        size_t first_space = line.find(' ');
        if (first_space != std::string::npos) {
            std::string cmd = line.substr(0, first_space);
            
            // Only complete program names for certain commands
            if (cmd == "start" || cmd == "stop" || cmd == "restart" || 
                cmd == "status" || cmd == "help") {
                matches = rl_completion_matches(text, program_name_generator);
            }
        }
    }
    
    return matches;
}

Shell::Shell() 
    : _handler(NULL), _running(false), _history(), _history_index(0) {
}

Shell::Shell(CommandHandler* cmd_handler)
    : _handler(cmd_handler), _running(false), _history(), _history_index(0) {
    setupReadline();
}

Shell::Shell(const Shell& other)
    : _handler(other._handler), _running(other._running),
      _history(other._history), _history_index(other._history_index) {
}

Shell& Shell::operator=(const Shell& other) {
    if (this != &other) {
        _handler = other._handler;
        _running = other._running;
        _history = other._history;
        _history_index = other._history_index;
    }
    return *this;
}

Shell::~Shell() {
    // Cleanup readline
    rl_callback_handler_remove();
}

void Shell::setupReadline() {
    // Set completion function
    rl_attempted_completion_function = shell_completion;
    
    // Set global handler for completion
    g_command_handler = _handler;
    
    // Load history from file if it exists
    read_history(".taskmaster_history");
}

void Shell::run() {
    _running = true;
    
    while (_running) {
        char* line = readline("taskmaster> ");
        
        if (line == NULL) {
            // EOF (Ctrl+D)
            std::cout << "\n";
            processCommand("quit");
            break;
        }
        
        std::string command(line);
        free(line);
        
        if (!command.empty()) {
            addToHistory(command);
            processCommand(command);
        }


        if (_handler->needsReload()) {
            _handler->clearReload();
            if (_onReload) {
                _onReload();
            }
        }
        
        if (_handler->needsShutdown()) {
            _running = false;
        }
    }
}

void Shell::stop() {
    _running = false;
}

void Shell::processCommand(const std::string& line) {
    if (line.empty()) return;
    
    _handler->execute(line);
}

bool Shell::checkInput(std::string& command) {
    // Non-blocking check for input using select
    fd_set readfds;
    struct timeval tv;
    
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    
    // No timeout - return immediately
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    
    int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv);
    
    if (ret > 0 && FD_ISSET(STDIN_FILENO, &readfds)) {
        // Input is available, read it
        if (std::getline(std::cin, command)) {
            return true;
        }
    }
    
    return false;
}

std::string Shell::readCommand() {
    // Not used in current implementation
    return "";
}

void Shell::addToHistory(const std::string& cmd) {
    if (!cmd.empty()) {
        add_history(cmd.c_str());
        _history.push_back(cmd);
        
        write_history(".taskmaster_history");
    }
}