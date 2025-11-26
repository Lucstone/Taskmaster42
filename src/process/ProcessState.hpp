#pragma once

enum class ProcessState {
    STOPPED,
    STARTING,
    RUNNING,
    BACKOFF,
    STOPPING,
    EXITED,
    FATAL
};

const char *processStateToString(ProcessState state);
