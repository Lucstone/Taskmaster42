#include "Logger.hpp"
#include <iostream>
#include <iomanip>

std::ofstream Logger::_stdoutFile;
std::ofstream Logger::_stderrFile;
std::mutex Logger::_mutex;

void Logger::init(const std::string& stdoutPath, const std::string& stderrPath) {
    std::lock_guard<std::mutex> lock(_mutex);

    if (!stdoutPath.empty()) {
        _stdoutFile.open(stdoutPath, std::ios::app);
        if (!_stdoutFile.is_open())
            std::cerr << "Logger: failed to open stdout log file: " << stdoutPath << std::endl;
    }

    if (!stderrPath.empty()) {
        _stderrFile.open(stderrPath, std::ios::app);
        if (!_stderrFile.is_open())
            std::cerr << "Logger: failed to open stderr log file: " << stderrPath << std::endl;
    }
}

void Logger::logEvent(const std::string& type, const std::string& name, pid_t pid, int code) {
    std::ostringstream oss;
    oss << "[" << nowString() << "] "
        << type << " name=" << name
        << " pid=" << pid
        << " code=" << code << "\n";

    std::string msg = oss.str();
    std::lock_guard<std::mutex> lock(_mutex);

    // Affiche aussi sur la sortie standard pour debug (optionnel)
    std::cout << msg;

    if (_stdoutFile.is_open())
        writeToFile(_stdoutFile, msg);
}

void Logger::logError(const std::string& message) {
    std::ostringstream oss;
    oss << "[" << nowString() << "] ERROR: " << message << "\n";
    std::string msg = oss.str();

    std::lock_guard<std::mutex> lock(_mutex);
    std::cerr << msg;

    if (_stderrFile.is_open())
        writeToFile(_stderrFile, msg);
}

std::string Logger::nowString() {
    std::time_t t = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return std::string(buf);
}

void Logger::writeToFile(std::ofstream& file, const std::string& msg) {
    file << msg;
    file.flush();
}
