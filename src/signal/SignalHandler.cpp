#include "SignalHandler.hpp"
#include "../logger/Logger.hpp"
#include <cstring>
#include <iostream>

volatile sig_atomic_t   SignalHandler::_sighup_received = 0;
volatile sig_atomic_t   SignalHandler::_sigchld_received = 0;
volatile sig_atomic_t   SignalHandler::_shutdown_requested = 0;

SignalHandler::SignalHandler() {}
SignalHandler::SignalHandler(const SignalHandler &other) { (void)other; }
SignalHandler &SignalHandler::operator=(const SignalHandler &other) {
    (void)other;
    return *this;
}
SignalHandler::~SignalHandler() {}

void SignalHandler::handleSighup(int sig) {
    (void)sig;
    _sighup_received = 1;
}

void SignalHandler::handleSigchld(int sig) {
    (void)sig;
    _sigchld_received = 1;
}

void SignalHandler::handleSigterm(int sig) {
    (void)sig;
    _shutdown_requested = 1;
}

void SignalHandler::handleSigint(int sig) {
    (void)sig;
    _shutdown_requested = 1;
}

void SignalHandler::setup() {
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));

    sa.sa_handler = handleSighup;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        std::cerr << "Error: Failed to setup SIGHUP: " << strerror(errno) << "\n";
        LOG_ERROR("Failed to setup SIGHUP: " + std::string(strerror(errno)));
    }

    sa.sa_handler = handleSigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        std::cerr << "Error: Failed to setup SIGCHLD: " << strerror(errno) << "\n";
        LOG_ERROR("Failed to setup SIGCHLD: " + std::string(strerror(errno)));
    }

    sa.sa_handler = handleSigterm;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        std::cerr << "Error: Failed to setup SIGTERM: " << strerror(errno) << "\n";
        LOG_ERROR("Failed to setup SIGTERM: " + std::string(strerror(errno)));
    }

    signal(SIGPIPE, SIG_IGN);
}

bool SignalHandler::needsConfigReload() {
    return _sighup_received != 0;
}

void SignalHandler::clearConfigReload() {
    _sighup_received = 0;
}

bool SignalHandler::needsProcessCheck() {
    return _sigchld_received != 0;
}

void SignalHandler::clearProcessCheck() {
    _sigchld_received = 0;
}

bool SignalHandler::shouldShutdown() {
    return _shutdown_requested != 0;
}

void SignalHandler::clearShutdown() {
    _shutdown_requested = 0;
}
