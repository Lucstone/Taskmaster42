#pragma once

#include <csignal>

class SignalHandler {
private:
    static volatile sig_atomic_t _sighup_received;
    static volatile sig_atomic_t _sigchld_received;
    static volatile sig_atomic_t _shutdown_requested;
    
    static void handleSighup(int sig);
    static void handleSigchld(int sig);
    static void handleSigterm(int sig);
    static void handleSigint(int sig);
    
    // Private constructor (singleton-like, all static)
    SignalHandler();
    SignalHandler(const SignalHandler& other);
    SignalHandler& operator=(const SignalHandler& other);
    ~SignalHandler();

public:
    // Setup all signal handlers
    static void setup();
    
    // Check and clear signal flags
    static bool needsConfigReload();
    static void clearConfigReload();
    
    static bool needsProcessCheck();
    static void clearProcessCheck();
    
    static bool shouldShutdown();
    static void clearShutdown();
};