#include "Utils.hpp"
#include <cstdlib>
#include <csignal>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <unistd.h>
#include <cctype>
#include <vector>
#include <string>
#include <thread>

int strToOctalUmask(const std::string& u) {
    char* end = nullptr;
    long v = std::strtol(u.c_str(), &end, 8);
    if (end == u.c_str() || v < 0 || v > 0777) return 0022;
    return static_cast<int>(v);
}

int signalFromString(const std::string& s) {
    if (s == "TERM") return SIGTERM;
    if (s == "INT")  return SIGINT;
    if (s == "KILL") return SIGKILL;
    if (s == "HUP")  return SIGHUP;
    if (s == "USR1") return SIGUSR1;
    if (s == "USR2") return SIGUSR2;
    return SIGTERM;
}

void splitCmd(const std::string& cmd, std::vector<char*>& argvStore) {
    static thread_local std::vector<std::string> toks;
    toks.clear();

    std::string cur;
    for (char c : cmd) {
        if (std::isspace(static_cast<unsigned char>(c))) {
            if (!cur.empty()) { toks.push_back(cur); cur.clear(); }
        } else cur.push_back(c);
    }
    if (!cur.empty()) toks.push_back(cur);

    argvStore.clear();
    argvStore.reserve(toks.size() + 1);
    for (auto& t : toks) argvStore.push_back(const_cast<char*>(t.c_str()));
    argvStore.push_back(nullptr);
}

int open_for_redirect(const std::string& path) {
    return ::open(path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
}

std::string nowString() {
    std::time_t t = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return std::string(buf);
}
