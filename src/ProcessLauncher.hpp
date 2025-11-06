#pragma once
#include "ConfigParser.hpp"
#include <vector>
#include <string>

class ProcessLauncher {
public:
    pid_t spawnOne(const ProgramConfig& cfg);
};
