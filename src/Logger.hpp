#pragma once
#include <string>
#include <iostream>
#include <ctime>

class Logger {
public:
    static void log(const std::string& type, const std::string& name, pid_t pid, int code) {
        std::cout << "[" << nowString() << "] "
                  << type << " name=" << name
                  << " pid=" << pid
                  << " code=" << code << "\n";
    }

private:
    static std::string nowString() {
        std::time_t t = std::time(nullptr);
        char buf[64]; 
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
        return std::string(buf);
    }
};
