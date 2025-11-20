#include "Logger.hpp"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

// Static member initialization
Logger* Logger::_instance = NULL;

// Private constructors (singleton)
Logger::Logger()
    : _log_file(),
      _min_level(LogLevel::INFO),
      _filename("logs/taskmaster.log") {
}

Logger::Logger(const std::string& filename)
    : _log_file(filename.c_str(), std::ios::app),
      _min_level(LogLevel::INFO),
      _filename(filename) {
}

// Delete copy constructor and assignment
Logger::Logger(const Logger& other) {
    (void)other;
    // Intentionally not implemented - singleton
}

Logger& Logger::operator=(const Logger& other) {
    (void)other;
    // Intentionally not implemented - singleton
    return *this;
}

// Destructor
Logger::~Logger() {
    if (_log_file.is_open()) {
        _log_file.close();
    }
}

// Get singleton instance
Logger* Logger::getInstance(const std::string& filename) {
    if (_instance == NULL) {
        _instance = new Logger(filename);
    }
    return _instance;
}

// Destroy singleton instance
void Logger::destroyInstance() {
    if (_instance != NULL) {
        delete _instance;
        _instance = NULL;
    }
}

// Get current timestamp
std::string Logger::getTimestamp() const {
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    return std::string(buffer);
}

// Convert log level to string
std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

// Main logging method
void Logger::log(LogLevel level, const std::string& message) {
    // Check if message should be logged based on minimum level
    if (level < _min_level) {
        return;
    }
    
    // Format: [TIMESTAMP] LEVEL: message
    std::ostringstream oss;
    oss << "[" << getTimestamp() << "] "
        << std::setw(7) << std::left << levelToString(level) << ": "
        << message;
    
    std::string formatted = oss.str();
    
    // Write to file if open
    if (_log_file.is_open()) {
        _log_file << formatted << std::endl;
        _log_file.flush(); // Ensure immediate write
    } else {
        // Fallback to stderr if file not available
        std::cerr << formatted << std::endl;
    }
}

// Convenience methods
void Logger::debug(const std::string& msg) {
    log(LogLevel::DEBUG, msg);
}

void Logger::info(const std::string& msg) {
    log(LogLevel::INFO, msg);
}

void Logger::warning(const std::string& msg) {
    log(LogLevel::WARNING, msg);
}

void Logger::error(const std::string& msg) {
    log(LogLevel::ERROR, msg);
}

// Set minimum log level
void Logger::setMinLevel(LogLevel level) {
    _min_level = level;
}

// Check if log file is open
bool Logger::isOpen() const {
    return _log_file.is_open();
}
