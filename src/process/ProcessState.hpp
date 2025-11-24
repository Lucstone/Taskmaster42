#pragma once

enum class ProcessState {
    STOPPED,    // Not running
    STARTING,   // Just spawned, waiting for starttime
    RUNNING,    // Running and past starttime
    BACKOFF,    // Failed to start, waiting before retry
    STOPPING,   // Received stop signal, waiting for graceful exit
    EXITED,     // Exited (expected exit code)
    FATAL       // Failed after max retries
};

const char* processStateToString(ProcessState state);