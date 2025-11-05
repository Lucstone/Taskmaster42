/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProgramConfig.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 06:36:16 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/04 07:29:58 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ProgramConfig.hpp"

ProgramConfig::ProgramConfig (void) : _name(""), _cmd(""), _umask("022"), _autorestart("unexpected"), _stopsignals("TERM")
, _stdout(""), _stderr(""), _workingdir(""), _exitcodes(1, 0)
, _env(), _autostart(false), _numprocs(1), _startretries(3), _starttime(5), _stoptime(10)
{
    //std::cout << "ProgramConfig Constructor is called" << std::endl;
    return;
}

ProgramConfig ::~ProgramConfig (void) {
    //std::cout << "ProgramConfig Destructor is called" << std::endl;
    return;
}

const std::string& ProgramConfig::getName() const 
{ 
    return this->_name; 
}

const std::string& ProgramConfig::getCmd() const 
{ 
    return this->_cmd; 
}

const std::string& ProgramConfig::getUmask() const 
{ 
    return this->_umask; 
}

const std::string& ProgramConfig::getAutorestart() const 
{ 
    return this->_autorestart; 
}

const std::string& ProgramConfig::getStopsignals() const 
{ 
    return this->_stopsignals; 
}

const std::string& ProgramConfig::getStdoutPath() const 
{ 
    return this->_stdout; 
}

const std::string& ProgramConfig::getStderrPath() const 
{ 
    return this->_stderr; 
}

const std::string& ProgramConfig::getWorkingDir() const 
{ 
    return this->_workingdir; 
}

const std::vector<int>& ProgramConfig::getExitcodes() const 
{ 
    return this->_exitcodes; 
}

const std::map<std::string,std::string>& ProgramConfig::getEnv() const 
{ 
    return this->_env; 
}

bool ProgramConfig::getAutostart() const 
{ 
    return this->_autostart; 
}

int  ProgramConfig::getNumprocs() const 
{ 
    return this->_numprocs; 
}

int  ProgramConfig::getStartretries() const 
{ 
    return this->_startretries; 
}

int  ProgramConfig::getStarttime() const 
{ 
    return this->_starttime; 
}

int  ProgramConfig::getStoptime() const 
{ 
    return this->_stoptime; 
}

void ProgramConfig::setName(const std::string& src) 
{
    this->_name = src; 
}

void ProgramConfig::setCmd(const std::string& src) 
{ 
    this->_cmd = src; 
}

void ProgramConfig::setUmask(const std::string& src) 
{
    this->_umask = src;
}

void ProgramConfig::setAutorestart(const std::string& src) 
{ 
    this->_autorestart = src; 
}

void ProgramConfig::setStopsignals(const std::string& src) 
{ 
    this->_stopsignals = src; 
}

void ProgramConfig::setStdoutPath(const std::string& src) 
{ 
    this->_stdout = src; 
}

void ProgramConfig::setStderrPath(const std::string& src) 
{ 
    this->_stderr = src; 
}

void ProgramConfig::setWorkingDir(const std::string& src) 
{ 
    this->_workingdir = src; 
}

void ProgramConfig::setExitcodes(const std::vector<int>& src) 
{ 
    this->_exitcodes = src; 
}

void ProgramConfig::setEnv(const std::map<std::string,std::string>& src) 
{ 
    this->_env = src; 
}

void ProgramConfig::setAutostart(bool src) 
{ 
    this->_autostart = src; 
}

void ProgramConfig::setNumprocs(int src) 
{ 
    this->_numprocs = src; 
}

void ProgramConfig::setStartretries(int src) 
{ 
    this->_startretries = src; 
}

void ProgramConfig::setStarttime(int src) 
{ 
    this->_starttime = src; 
}

void ProgramConfig::setStoptime(int src) 
{ 
    this->_stoptime = src; 
}

const char* ProgramConfig::InvalidException::what() const throw()
{
	return("Missing or invalid section in config");
}

static std::string ToString(const YAML::Node& node)
{
    try 
    {
        return node.as<std::string>();
    } 
    catch (const std::exception&) 
    {
        std::ostringstream oss;
        oss << node;
        return oss.str();
    }
}

static std::string readCmdAsString(const YAML::Node& cmdNode)
{
    if (!cmdNode)
        throw ProgramConfig::InvalidException();
    if (cmdNode.Type() == YAML::NodeType::Scalar)
        return cmdNode.as<std::string>();
    if (cmdNode.Type() == YAML::NodeType::Sequence) 
    {
        std::ostringstream oss;
        bool first = true;

        for (const auto& it : cmdNode) 
        {
            if (!first) oss << ' ';
            oss << it.as<std::string>();
            first = false;
        }
        return oss.str();
    }
    throw ProgramConfig::InvalidException();
}

ProgramConfig ProgramConfig::fromYAML(const std::string& name, const YAML::Node& node)
{
    ProgramConfig config;

    config.setName(name);

    if (!node["cmd"])
        throw ProgramConfig::InvalidException();
    config.setCmd(readCmdAsString(node["cmd"]));
    if (node["numprocs"]) 
        config.setNumprocs(node["numprocs"].as<int>());
    if (node["autostart"]) 
        config.setAutostart(node["autostart"].as<bool>());
    if (node["autorestart"]) 
        config.setAutorestart(node["autorestart"].as<std::string>());
    if (node["umask"]) 
        config.setUmask(ToString(node["umask"]));
    if (node["stopsignal"]) 
        config.setStopsignals(node["stopsignal"].as<std::string>());
    if (node["stdout"]) 
        config.setStdoutPath(node["stdout"].as<std::string>());
    if (node["stderr"]) 
        config.setStderrPath(node["stderr"].as<std::string>());
    if (node["workingdir"]) 
        config.setWorkingDir(node["workingdir"].as<std::string>());
    if (node["exitcodes"]) 
    {
        std::vector<int> codes;
        YAML::Node ex = node["exitcodes"];

        if (ex.Type() == YAML::NodeType::Sequence) {
            for (YAML::const_iterator it = ex.begin(); it != ex.end(); ++it) 
            {
                codes.push_back(it->as<int>());
            }
        } 
        else 
        {
            codes.push_back(ex.as<int>());
        }

        config.setExitcodes(codes);
    }
    if (node["startretries"]) 
        config.setStartretries(node["startretries"].as<int>());
    if (node["starttime"])    
        config.setStarttime(node["starttime"].as<int>());
    if (node["stoptime"])     
        config.setStoptime(node["stoptime"].as<int>());
    if (node["env"]) 
    {
        std::map<std::string, std::string> env;
        for (auto it : node["env"])
            env[it.first.as<std::string>()] = ToString(it.second);
        config.setEnv(env);
    }
    if (config.getCmd().empty())
        throw ProgramConfig::InvalidException();
    if (config.getNumprocs() <= 0)
        throw ProgramConfig::InvalidException();
    if (config.getStartretries() < 0)
        throw ProgramConfig::InvalidException();
    return config;
}

bool NodeIsMap(const YAML::Node& node) {
    return node && node.Type() == YAML::NodeType::Map;
}

std::map<std::string, ProgramConfig> ProgramConfig::loadAll(const std::string& path)
{
    YAML::Node node = YAML::LoadFile(path);
    std::map<std::string, ProgramConfig> config;

    if (!NodeIsMap(node))
        throw ProgramConfig::InvalidException();
    
    for (std::pair<YAML::Node, YAML::Node> it : node["programs"]) 
    {
        std::string name = it.first.as<std::string>();
        config[name] = ProgramConfig::fromYAML(name, it.second);
    }
    return config;
}