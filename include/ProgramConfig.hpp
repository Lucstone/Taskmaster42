/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProgramConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 06:36:06 by lnaidu            #+#    #+#             */
/*   Updated: 2025/10/17 07:50:02 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
        std::string                         _unmask;
        std::string                         _autorestart;
        std::string                         _stopsignals;
        std::string                         _stdout;
        std::string                         _stderr;
        std::vector<int>                    _exitcodes;
        std::map<std::string, std::string>  _env;
        bool                                _autostart;
        int                                 _numprocs;
        int                                 _startretries;
        int                                 _starttime;
        int                                 _stoptime;

    public :
        ProgramConfig() = default;
        
};