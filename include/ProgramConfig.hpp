/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProgramConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 06:36:06 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/03 06:58:27 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

# include <string>
# include <vector>
# include <map>
# include <iostream>
# include <cstdlib>   
# include <yaml-cpp/yaml.h>



class ProgramConfig {
    
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
        ProgramConfig();
        ~ProgramConfig();
        
        const std::string&                  getName()        const;
        const std::string&                  getCmd()         const;
        const std::string&                  getUmask()       const;
        const std::string&                  getAutorestart() const;
        const std::string&                  getStopsignals() const;
        const std::string&                  getStdoutPath()  const;
        const std::string&                  getStderrPath()  const;
        const std::string&                  getWorkingDir()  const;
        const std::vector<int>&             getExitcodes()   const;
        const std::map<std::string,std::string>& getEnv()    const;
        bool                                getAutostart()   const;
        int                                 getNumprocs()    const;
        int                                 getStartretries()const;
        int                                 getStarttime()   const;
        int                                 getStoptime()    const;
        void                                setName(const std::string& v);
        void                                setCmd(const std::string& v);
        void                                setUmask(const std::string& v);
        void                                setAutorestart(const std::string& v);
        void                                setStopsignals(const std::string& v);
        void                                setStdoutPath(const std::string& v);
        void                                setStderrPath(const std::string& v);
        void                                setWorkingDir(const std::string& v);
        void                                setExitcodes(const std::vector<int>& v);
        void                                setEnv(const std::map<std::string,std::string>& v);
        void                                setAutostart(bool v);
        void                                setNumprocs(int v);
        void                                setStartretries(int v);
        void                                setStarttime(int v);
        void                                setStoptime(int v);
        
        static ProgramConfig                fromYAML(const std::string& name, const YAML::Node& node);
        static std::map<std::string, ProgramConfig> loadAll(const std::string& path);
};