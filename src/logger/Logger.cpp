#include "Logger.hpp"
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

Logger  *Logger::_instance = NULL;

Logger::Logger()
    : _log_file(),
      _min_level(LogLevel::INFO),
      _filename("logs/taskmaster.log") {
}

Logger::Logger(const std::string &filename)
    : _log_file(filename.c_str(), std::ios::app),
      _min_level(LogLevel::INFO),
      _filename(filename) {
}

Logger::Logger(const Logger &other) {
    (void)other;
}

Logger  &Logger::operator=(const Logger &other) {
    (void)other;
    return *this;
}

Logger::~Logger() {
    if (_log_file.is_open()) {
        _log_file.close();
    }
}

Logger  *Logger::getInstance(const std::string &filename) {
    if (_instance == NULL) {
        _instance = new Logger(filename);
    }
    return _instance;
}

void Logger::destroyInstance() {
    if (_instance != NULL) {
        delete _instance;
        _instance = NULL;
    }
}

std::string Logger::getTimestamp() const {
    time_t      now = time(NULL);
    struct tm   *timeinfo = localtime(&now);

    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    return std::string(buffer);
}

std::string Logger::levelToString(LogLevel level) const {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}

void Logger::log(LogLevel level, const std::string &message) {
    if (level < _min_level) {
        return;
    }

    std::ostringstream oss;
    oss << "[" << getTimestamp() << "] "
        << std::setw(7) << std::left << levelToString(level) << ": "
        << message;

    std::string formatted = oss.str();

    if (_log_file.is_open()) {
        _log_file << formatted << std::endl;
        _log_file.flush();
    } else {
        std::cerr << formatted << std::endl;
    }
}

void Logger::debug(const std::string &msg) {
    log(LogLevel::DEBUG, msg);
}

void Logger::info(const std::string &msg) {
    log(LogLevel::INFO, msg);
}

void Logger::warning(const std::string &msg) {
    log(LogLevel::WARNING, msg);
}

void Logger::error(const std::string &msg) {
    log(LogLevel::ERROR, msg);
}

void Logger::setMinLevel(LogLevel level) {
    _min_level = level;
}

bool Logger::isOpen() const {
    return _log_file.is_open();
}
