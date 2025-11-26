#pragma once

#include <string>
#include <fstream>

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

class Logger {
private:
    static Logger   *_instance;
    std::ofstream   _log_file;
    LogLevel        _min_level;
    std::string     _filename;

    Logger();
    Logger(const std::string &filename);
    Logger(const Logger &other);
    Logger &operator=(const Logger &other);

    std::string     getTimestamp() const;
    std::string     levelToString(LogLevel level) const;

public:
    ~Logger();

    static Logger   *getInstance(const std::string& filename = "logs/taskmaster.log");

    static void     destroyInstance();

    void            log(LogLevel level, const std::string &message);
    void            debug(const std::string &msg);
    void            info(const std::string &msg);
    void            warning(const std::string &msg);
    void            error(const std::string &msg);

    void            setMinLevel(LogLevel level);
    bool            isOpen() const;
};

#define LOG_DEBUG(msg) Logger::getInstance()->debug(msg)
#define LOG_INFO(msg) Logger::getInstance()->info(msg)
#define LOG_WARNING(msg) Logger::getInstance()->warning(msg)
#define LOG_ERROR(msg) Logger::getInstance()->error(msg)
