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
    static Logger* _instance;
    std::ofstream _log_file;
    LogLevel _min_level;
    std::string _filename;
    
    // Private constructor for singleton
    Logger();
    Logger(const std::string& filename);
    
    // Delete copy constructor and assignment (singleton)
    Logger(const Logger& other);
    Logger& operator=(const Logger& other);
    
    std::string getTimestamp() const;
    std::string levelToString(LogLevel level) const;

public:
    // Destructor
    ~Logger();
    
    // Get singleton instance
    static Logger* getInstance(const std::string& filename = "logs/taskmaster.log");
    
    // Destroy singleton instance
    static void destroyInstance();
    
    // Logging methods
    void log(LogLevel level, const std::string& message);
    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warning(const std::string& msg);
    void error(const std::string& msg);
    
    // Configuration
    void setMinLevel(LogLevel level);
    bool isOpen() const;
};

// Convenience macros
#define LOG_DEBUG(msg) Logger::getInstance()->debug(msg)
#define LOG_INFO(msg) Logger::getInstance()->info(msg)
#define LOG_WARNING(msg) Logger::getInstance()->warning(msg)
#define LOG_ERROR(msg) Logger::getInstance()->error(msg)