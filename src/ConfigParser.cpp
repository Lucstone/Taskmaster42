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

ConfigParser::ConfigParser (void) : _name(""), _cmd(""), _umask("022"), _autorestart("unexpected"), _stopsignals("TERM")
, _stdout(""), _stderr(""), _workingdir(""), _exitcodes(1, 0)
, _env(), _autostart(false), _numprocs(1), _startretries(3), _starttime(5), _stoptime(10)
{
    //std::cout << "ConfigParser Constructor is called" << std::endl;
    return;
}

ConfigParser ::~ConfigParser (void) {
    //std::cout << "ConfigParser Destructor is called" << std::endl;
    return;
}

const std::string& ConfigParser::getName() const 
{ 
    return this->_name; 
}

const std::string& ConfigParser::getCmd() const 
{ 
    return this->_cmd; 
}

const std::string& ConfigParser::getUmask() const 
{ 
    return this->_umask; 
}

const std::string& ConfigParser::getAutorestart() const 
{ 
    return this->_autorestart; 
}

const std::string& ConfigParser::getStopsignals() const 
{ 
    return this->_stopsignals; 
}

const std::string& ConfigParser::getStdoutPath() const 
{ 
    return this->_stdout; 
}

const std::string& ConfigParser::getStderrPath() const 
{ 
    return this->_stderr; 
}

const std::string& ConfigParser::getWorkingDir() const 
{ 
    return this->_workingdir; 
}

const std::vector<int>& ConfigParser::getExitcodes() const 
{ 
    return this->_exitcodes; 
}

const std::map<std::string,std::string>& ConfigParser::getEnv() const 
{ 
    return this->_env; 
}

bool ConfigParser::getAutostart() const 
{ 
    return this->_autostart; 
}

int  ConfigParser::getNumprocs() const 
{ 
    return this->_numprocs; 
}

int  ConfigParser::getStartretries() const 
{ 
    return this->_startretries; 
}

int  ConfigParser::getStarttime() const 
{ 
    return this->_starttime; 
}

int  ConfigParser::getStoptime() const 
{ 
    return this->_stoptime; 
}

void ConfigParser::setName(const std::string& src) 
{
    this->_name = src; 
}

void ConfigParser::setCmd(const std::string& src) 
{ 
    this->_cmd = src; 
}

void ConfigParser::setUmask(const std::string& src) 
{
    this->_umask = src;
}

void ConfigParser::setAutorestart(const std::string& src) 
{ 
    this->_autorestart = src; 
}

void ConfigParser::setStopsignals(const std::string& src) 
{ 
    this->_stopsignals = src; 
}

void ConfigParser::setStdoutPath(const std::string& src) 
{ 
    this->_stdout = src; 
}

void ConfigParser::setStderrPath(const std::string& src) 
{ 
    this->_stderr = src; 
}

void ConfigParser::setWorkingDir(const std::string& src) 
{ 
    this->_workingdir = src; 
}

void ConfigParser::setExitcodes(const std::vector<int>& src) 
{ 
    this->_exitcodes = src; 
}

void ConfigParser::setEnv(const std::map<std::string,std::string>& src) 
{ 
    this->_env = src; 
}

void ConfigParser::setAutostart(bool src) 
{ 
    this->_autostart = src; 
}

void ConfigParser::setNumprocs(int src) 
{ 
    this->_numprocs = src; 
}

void ConfigParser::setStartretries(int src) 
{ 
    this->_startretries = src; 
}

void ConfigParser::setStarttime(int src) 
{ 
    this->_starttime = src; 
}

void ConfigParser::setStoptime(int src) 
{ 
    this->_stoptime = src; 
}

const char* ConfigParser::InvalidException::what() const throw()
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
        throw ConfigParser::InvalidException();
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
    throw ConfigParser::InvalidException();
}

ConfigParser ConfigParser::fromYAML(const std::string& name, const YAML::Node& node)
{
    ConfigParser config;

    config.setName(name);

    if (!node["cmd"])
        throw ConfigParser::InvalidException();
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
        throw ConfigParser::InvalidException();
    if (config.getNumprocs() <= 0)
        throw ConfigParser::InvalidException();
    if (config.getStartretries() < 0)
        throw ConfigParser::InvalidException();
    return config;
}

bool NodeIsMap(const YAML::Node& node) {
    return node && node.Type() == YAML::NodeType::Map;
}

std::map<std::string, ConfigParser> ConfigParser::loadAll(const std::string& path)
{
    YAML::Node node = YAML::LoadFile(path);
    std::map<std::string, ConfigParser> config;

    if (!NodeIsMap(node))
        throw ConfigParser::InvalidException();
    
    for (std::pair<YAML::Node, YAML::Node> it : node["programs"]) 
    {
        std::string name = it.first.as<std::string>();
        config[name] = ConfigParser::fromYAML(name, it.second);
    }
    return config;
}