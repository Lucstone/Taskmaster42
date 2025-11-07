#pragma once
#include <string>
#include <map>
#include "ProcessInfo.hpp"
#include "ConfigParser.hpp"

class ConfigManager {
private:
    std::map<std::string, ConfigParser> _configs;

public:
    const std::map<std::string, ConfigParser>& getConfigs() const { return _configs; }

    void ConfigManager::loadConfig(const std::string& path) {
        _configs = ConfigParser::parse(path);

        for (auto& kv : _configs) {
            const std::string& name = kv.first;
            const ProgramConfig& cfg = kv.second;

            // Toujours créer l'entrée dans _table, STOPPED par défaut
            if (_table.find(name) == _table.end()) {
                _table[name] = std::vector<ProcessInfo>();
                _table[name].push_back(ProcessInfo(-1));
            }
        }
    }

    void ConfigManager::loadConfig(const std::string& path) {
        _configs = ConfigParser::parse(path);

        for (auto& kv : _configs) {
            const std::string& name = kv.first;
            const ProgramConfig& cfg = kv.second;

            // Crée toujours l'entrée dans _table
            if (_table.find(name) == _table.end()) {
                _table[name] = std::vector<ProcessInfo>();
                _table[name].push_back(ProcessInfo(-1)); // état "STOPPED / Not started"
            }

            // Lancer uniquement si autostart=true
            if (cfg.getAutostart()) {
                startProgram(name);
            }
        }
    }

    void ProcessManager::reloadConfig(const std::string& path) {
        auto fresh = ConfigParser::parse(path); // nouveaux configs

        // Stop et remove les programmes qui ont disparu
        for (auto it = _configs.begin(); it != _configs.end(); ++it) {
            if (!fresh.count(it->first)) {
                stopProgram(it->first);
                logEvent("removed", it->first, -1, 0);
                _table.erase(it->first);
            }
        }

        // Ajout / mise à jour des programmes
        for (auto& kv : fresh) {
            const std::string& name = kv.first;
            const ProgramConfig& cfg = kv.second;

            if (_table.find(name) == _table.end()) {
                _table[name].push_back(ProcessInfo(-1)); // STOPPED / Not started
            }

            // Si autostart est vrai et que le programme n'est pas encore en cours
            if (cfg.getAutostart() && !_table[name].back().isRunning()) {
                startProgram(name);
            }
        }

        _configs.swap(fresh);
        logEvent("reload", "-", -1, 0);
    }
};
