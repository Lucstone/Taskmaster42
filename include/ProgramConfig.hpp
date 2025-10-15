/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ProgramConfig.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 06:36:06 by lnaidu            #+#    #+#             */
/*   Updated: 2025/10/15 08:58:39 by lnaidu           ###   ########.fr       */
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
        std::string _name;
        std::string _cmd;
        std::vector<std::string> _args;




    public :
        ProgramConfig() = default;
        
};