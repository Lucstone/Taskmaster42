#pragma once
#include <string>
#include <ctime>
#include <fstream>
#include <mutex>

class Logger {
public:
    // Initialise les chemins des fichiers de log (stdout / stderr)
    static void init(const std::string& stdoutPath, const std::string& stderrPath);

    // Log un événement (ex: start, stop, reload)
    static void logEvent(const std::string& type, const std::string& name, pid_t pid, int code = 0);

    // Log un message d'erreur
    static void logError(const std::string& message);

private:
    static std::string nowString();
    static void writeToFile(std::ofstream& file, const std::string& msg);

    static std::ofstream _stdoutFile;
    static std::ofstream _stderrFile;
    static std::mutex _mutex;
};
