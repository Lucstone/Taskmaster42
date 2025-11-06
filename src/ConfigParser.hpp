/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 06:36:06 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/04 06:28:19 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <string>
# include <vector>
# include <map>
# include <iostream>
# include <cstdlib>   
# include <yaml-cpp/yaml.h>



class ConfigParser {
    
    private :
        std::string                         _name;
        std::string                         _cmd;
        std::string                         _umask;
        std::string                         _autorestart;
        std::string                         _stopsignals;
        std::string                         _stdout;
        std::string                         _stderr;
        std::string                         _workingdir;
        std::vector<int>                    _exitcodes;
        std::map<std::string, std::string>  _env;
        bool                                _autostart;
        int                                 _numprocs;
        int                                 _startretries;
        int                                 _starttime;
        int                                 _stoptime;

    public :
        ConfigParser();
        ~ConfigParser();
        
        const std::string   &getName()   const;
        const std::string   &getCmd()    const;
        const std::string   &getUmask()  const;
        const std::string   &getAutorestart()    const;
        const std::string   &getStopsignals()    const;
        const std::string   &getStdoutPath() const;
        const std::string   &getStderrPath() const;
        const std::string   &getWorkingDir() const;
        const std::vector<int>  &getExitcodes()  const;
        const std::map<std::string,std::string> &getEnv()    const;
        bool    getAutostart()   const;
        int     getNumprocs()    const;
        int     getStartretries()const;
        int     getStarttime()   const;
        int     getStoptime()    const;
        void    setName(const std::string& src);
        void    setCmd(const std::string& src);
        void    setUmask(const std::string& src);
        void    setAutorestart(const std::string& src);
        void    setStopsignals(const std::string& src);
        void    setStdoutPath(const std::string& src);
        void    setStderrPath(const std::string& src);
        void    setWorkingDir(const std::string& src);
        void    setExitcodes(const std::vector<int>& src);
        void    setEnv(const std::map<std::string,std::string>& src);
        void    setAutostart(bool src);
        void    setNumprocs(int src);
        void    setStartretries(int src);
        void    setStarttime(int src);
        void    setStoptime(int src);
        
        static ConfigParser    fromYAML(const std::string& name, const YAML::Node& node);
        static std::map<std::string, ConfigParser> loadAll(const std::string& path);
        class InvalidException : public std::exception
		{
			public :
				virtual const char* what() const throw();
		};
};