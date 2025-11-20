#include "Utils.hpp"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <ctime>
#include <iomanip>
#include <cstring>

// Private constructor (no instances allowed)
Utils::Utils() {}
Utils::Utils(const Utils& other) { (void)other; }
Utils& Utils::operator=(const Utils& other) {
    (void)other;
    return *this;
}
Utils::~Utils() {}

// ============================================
// String utilities
// ============================================

std::string Utils::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.length();
    
    // Find first non-whitespace
    while (start < end && std::isspace(str[start])) {
        ++start;
    }
    
    // Find last non-whitespace
    while (end > start && std::isspace(str[end - 1])) {
        --end;
    }
    
    return str.substr(start, end - start);
}

std::vector<std::string> Utils::split(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string Utils::join(const std::vector<std::string>& vec, const std::string& delimiter) {
    std::string result;
    
    for (size_t i = 0; i < vec.size(); ++i) {
        result += vec[i];
        if (i < vec.size() - 1) {
            result += delimiter;
        }
    }
    
    return result;
}

std::string Utils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string Utils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// ============================================
// Signal utilities
// ============================================

int Utils::signalNameToNumber(const std::string& signal_name) {
    std::string upper = toUpper(signal_name);
    
    // Remove "SIG" prefix if present
    if (upper.find("SIG") == 0) {
        upper = upper.substr(3);
    }
    
    if (upper == "HUP") return SIGHUP;
    if (upper == "INT") return SIGINT;
    if (upper == "QUIT") return SIGQUIT;
    if (upper == "ILL") return SIGILL;
    if (upper == "TRAP") return SIGTRAP;
    if (upper == "ABRT") return SIGABRT;
    if (upper == "BUS") return SIGBUS;
    if (upper == "FPE") return SIGFPE;
    if (upper == "KILL") return SIGKILL;
    if (upper == "USR1") return SIGUSR1;
    if (upper == "SEGV") return SIGSEGV;
    if (upper == "USR2") return SIGUSR2;
    if (upper == "PIPE") return SIGPIPE;
    if (upper == "ALRM") return SIGALRM;
    if (upper == "TERM") return SIGTERM;
    if (upper == "CHLD") return SIGCHLD;
    if (upper == "CONT") return SIGCONT;
    if (upper == "STOP") return SIGSTOP;
    if (upper == "TSTP") return SIGTSTP;
    if (upper == "TTIN") return SIGTTIN;
    if (upper == "TTOU") return SIGTTOU;
    
    // Default to SIGTERM
    return SIGTERM;
}

std::string Utils::signalNumberToName(int signal_num) {
    switch (signal_num) {
        case SIGHUP:  return "SIGHUP";
        case SIGINT:  return "SIGINT";
        case SIGQUIT: return "SIGQUIT";
        case SIGILL:  return "SIGILL";
        case SIGTRAP: return "SIGTRAP";
        case SIGABRT: return "SIGABRT";
        case SIGBUS:  return "SIGBUS";
        case SIGFPE:  return "SIGFPE";
        case SIGKILL: return "SIGKILL";
        case SIGUSR1: return "SIGUSR1";
        case SIGSEGV: return "SIGSEGV";
        case SIGUSR2: return "SIGUSR2";
        case SIGPIPE: return "SIGPIPE";
        case SIGALRM: return "SIGALRM";
        case SIGTERM: return "SIGTERM";
        case SIGCHLD: return "SIGCHLD";
        case SIGCONT: return "SIGCONT";
        case SIGSTOP: return "SIGSTOP";
        case SIGTSTP: return "SIGTSTP";
        case SIGTTIN: return "SIGTTIN";
        case SIGTTOU: return "SIGTTOU";
        default:      return "UNKNOWN";
    }
}

// ============================================
// File utilities
// ============================================

bool Utils::fileExists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool Utils::createDirectory(const std::string& path) {
    // Check if already exists
    if (fileExists(path)) {
        return true;
    }
    
    // Create directory with permissions 0755
    return (mkdir(path.c_str(), 0755) == 0);
}

std::string Utils::getAbsolutePath(const std::string& path) {
    char* resolved_path = realpath(path.c_str(), NULL);
    if (resolved_path == NULL) {
        return path; // Return original if resolution fails
    }
    
    std::string result(resolved_path);
    free(resolved_path);
    return result;
}

// ============================================
// Time utilities
// ============================================

std::string Utils::formatDuration(time_t seconds) {
    std::ostringstream oss;
    
    if (seconds < 60) {
        oss << seconds << "s";
    } else if (seconds < 3600) {
        oss << (seconds / 60) << "m " << (seconds % 60) << "s";
    } else if (seconds < 86400) {
        time_t hours = seconds / 3600;
        time_t mins = (seconds % 3600) / 60;
        oss << hours << "h " << mins << "m";
    } else {
        time_t days = seconds / 86400;
        time_t hours = (seconds % 86400) / 3600;
        oss << days << "d " << hours << "h";
    }
    
    return oss.str();
}

std::string Utils::getCurrentTimestamp() {
    time_t now = time(NULL);
    struct tm* timeinfo = localtime(&now);
    
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    return std::string(buffer);
}

// ============================================
// Process utilities
// ============================================

bool Utils::isProcessRunning(pid_t pid) {
    if (pid <= 0) {
        return false;
    }
    
    // Send signal 0 to check if process exists
    // Returns 0 if process exists, -1 if not
    return (kill(pid, 0) == 0);
}

void Utils::setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        return;
    }
    
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}