#pragma once
#include <string>
#include <vector>
#include <unordered_map>

class Helper {
public:
    Helper();

    void handle(const std::vector<std::string>& args) const;

private:
    std::vector<std::string> commands;
    std::unordered_map<std::string, std::string> helpTexts;

    bool is_known_command(const std::string& cmd) const;
    void print_default_help() const;
    void print_command_help(const std::vector<std::string>& args) const;
};