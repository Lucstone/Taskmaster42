#include "SignalHandler.hpp"
#include <cstring>

// Static member initialization
volatile sig_atomic_t SignalHandler::_sighup_received = 0;
volatile sig_atomic_t SignalHandler::_sigchld_received = 0;
volatile sig_atomic_t SignalHandler::_shutdown_requested = 0;

// Private constructors (singleton-like pattern)
SignalHandler::SignalHandler() {}
SignalHandler::SignalHandler(const SignalHandler& other) { (void)other; }
SignalHandler& SignalHandler::operator=(const SignalHandler& other) {
    (void)other;
    return *this;
}
SignalHandler::~SignalHandler() {}

// Signal handler for SIGHUP (reload configuration)
void SignalHandler::handleSighup(int sig) {
    (void)sig;
    _sighup_received = 1;
}

// Signal handler for SIGCHLD (child process terminated)
void SignalHandler::handleSigchld(int sig) {
    (void)sig;
    _sigchld_received = 1;
}

// Signal handler for SIGTERM (graceful shutdown)
void SignalHandler::handleSigterm(int sig) {
    (void)sig;
    _shutdown_requested = 1;
}

// Signal handler for SIGINT (Ctrl+C)
void SignalHandler::handleSigint(int sig) {
    (void)sig;
    _shutdown_requested = 1;
}

// Setup all signal handlers
void SignalHandler::setup() {
    struct sigaction sa;
    
    // Zero out the structure
    memset(&sa, 0, sizeof(sa));
    
    // Setup SIGHUP handler (reload configuration)
    sa.sa_handler = handleSighup;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; // Restart interrupted system calls
    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        // Error handling could be added here
    }
    
    // Setup SIGCHLD handler (child process terminated)
    sa.sa_handler = handleSigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP; // Don't notify on stop/continue
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        // Error handling could be added here
    }
    
    // Setup SIGTERM handler (graceful shutdown)
    sa.sa_handler = handleSigterm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        // Error handling could be added here
    }
    
    // Setup SIGINT handler (Ctrl+C)
    sa.sa_handler = handleSigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        // Error handling could be added here
    }
    
    // Ignore SIGPIPE (broken pipe)
    // This prevents the program from crashing if writing to a closed socket/pipe
    signal(SIGPIPE, SIG_IGN);
}

// Check if configuration reload requested
bool SignalHandler::needsConfigReload() {
    return _sighup_received != 0;
}

// Clear configuration reload flag
void SignalHandler::clearConfigReload() {
    _sighup_received = 0;
}

// Check if process check needed (SIGCHLD received)
bool SignalHandler::needsProcessCheck() {
    return _sigchld_received != 0;
}

// Clear process check flag
void SignalHandler::clearProcessCheck() {
    _sigchld_received = 0;
}

// Check if shutdown requested
bool SignalHandler::shouldShutdown() {
    return _shutdown_requested != 0;
}

// Clear shutdown flag
void SignalHandler::clearShutdown() {
    _shutdown_requested = 0;
}
