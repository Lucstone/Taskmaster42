#pragma once

#include <string>
#include <vector>
#include <sys/types.h>

class Utils {
private:
    Utils();
    Utils(const Utils &other);
    Utils &operator=(const Utils &other);
    ~Utils();

public:
    static std::string              trim(const std::string &str);
    static std::vector<std::string> split(const std::string &str, char delimiter);
    static std::string              join(const std::vector<std::string> &vec, const std::string &delimiter);
    static std::string              toLower(const std::string &str);
    static std::string              toUpper(const std::string &str);
    static int                      signalNameToNumber(const std::string &signal_name);
    static std::string              signalNumberToName(int signal_num);
    static bool                     fileExists(const std::string &path);
    static bool                     createDirectory(const std::string &path);
    static std::string              getAbsolutePath(const std::string &path);
    static std::string              formatDuration(time_t seconds);
    static std::string              getCurrentTimestamp();
    static bool                     isProcessRunning(pid_t pid);
    static void                     setNonBlocking(int fd);
};
