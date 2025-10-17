/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProgramConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 06:36:06 by lnaidu            #+#    #+#             */
/*   Updated: 2025/10/17 18:19:48 by lnaidu           ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

# include <string>
# include <vector>
# include <map>

# include <iostream>
# include <cstdlib>   

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
        std::string getName(void) const;
        std::string getCmd(void) const;
        std::string getUmask(void) const;
        std::string getAutorestart(void) const;
        std::string getStopsignals(void) const;
        
        
        // voir ave & 
        
        
};