#pragma once
#include <string>
#include <vector>

int strToOctalUmask(const std::string& u);
int signalFromString(const std::string& s);
void splitCmd(const std::string& cmd, std::vector<char*>& argvStore);
int open_for_redirect(const std::string& path);
std::string nowString();
std::string formatUptime(time_t seconds);
