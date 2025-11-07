/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 06:36:16 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/04 07:29:58 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"

#include <sstream>
#include <iostream>
#include <ctime>

namespace {
    std::string toString(const YAML::Node& node) {
        try {
            return node.as<std::string>();
        } catch (...) {
            std::ostringstream oss;
            oss << node;
            return oss.str();
        }
    }

    std::string readCmdAsString(const YAML::Node& cmdNode) {
        if (!cmdNode)
            throw ConfigParser::InvalidException();

        switch (cmdNode.Type()) {
            case YAML::NodeType::Scalar:
                return cmdNode.as<std::string>();
            case YAML::NodeType::Sequence: {
                std::ostringstream oss;
                bool first = true;
                for (const auto& it : cmdNode) {
                    if (!first) oss << ' ';
                    oss << it.as<std::string>();
                    first = false;
                }
                return oss.str();
            }
            default:
                throw ConfigParser::InvalidException();
        }
    }

    bool nodeIsMap(const YAML::Node& node) {
        return node && node.Type() == YAML::NodeType::Map;
    }
}

ConfigParser ConfigParser::fromYAML(const std::string& name, const YAML::Node& node) {
    ConfigParser    config;

    config.setName(name);

    if (!node["cmd"])
        throw InvalidException();
    config.setCmd(readCmdAsString(node["cmd"]));

    if (node["numprocs"])      config.setNumProcs(node["numprocs"].as<int>());
    if (node["autostart"])     config.setAutostart(node["autostart"].as<bool>());
    if (node["autorestart"])   config.setAutorestart(node["autorestart"].as<std::string>());
    if (node["umask"])         config.setUmask(toString(node["umask"]));
    if (node["stopsignal"])    config.setStopsignals(node["stopsignal"].as<std::string>());
    if (node["stdout"])        config.setStdoutPath(node["stdout"].as<std::string>());
    if (node["stderr"])        config.setStderrPath(node["stderr"].as<std::string>());
    if (node["workingdir"])    config.setWorkingDir(node["workingdir"].as<std::string>());
    if (node["exitcodes"]) {
        std::vector<int> codes;
        YAML::Node ex = node["exitcodes"];
        if (ex.Type() == YAML::NodeType::Sequence) {
            for (auto it = ex.begin(); it != ex.end(); ++it)
                codes.push_back(it->as<int>());
        } else {
            codes.push_back(ex.as<int>());
        }
        config.setExitcodes(codes);
    }

    if (node["startretries"]) config.setStartRetries(node["startretries"].as<int>());
    if (node["starttime"])    config.setStartTime(node["starttime"].as<int>());
    if (node["stoptime"])     config.setStopTime(node["stoptime"].as<int>());

    if (node["env"]) {
        std::map<std::string,std::string> env;
        for (auto it : node["env"])
            env[it.first.as<std::string>()] = toString(it.second);
        config.setEnv(env);
    }

    // Vérifications simples
    if (config.getCmd().empty() || config.getNumProcs() <= 0 || config.getStartRetries() < 0)
        throw InvalidException();

    return config;
}

std::map<std::string, ConfigParser> ConfigParser::loadAll(const std::string &path) {
    YAML::Node root = YAML::LoadFile(path);

    if (!nodeIsMap(root))
        throw InvalidException();

    std::map<std::string, ConfigParser> configs;
    YAML::Node                          programs = root["programs"];

    if (!programs)
        throw InvalidException();

    for (auto it : programs) {
        std::string name = it.first.as<std::string>();
        configs[name] = ConfigParser::fromYAML(name, it.second);
    }

    return configs;
}
