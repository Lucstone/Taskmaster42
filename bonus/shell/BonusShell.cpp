/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BonusShell.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 07:25:49 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/26 04:23:55 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "BonusShell.hpp"

#include <iostream>
#include <cstdlib>          
#include <string>

#include <readline/readline.h>
#include <readline/history.h>

BonusShell::BonusShell() {}

BonusShell::BonusShell(const BonusShell& other) : _client(other._client) {}

BonusShell& BonusShell::operator=(const BonusShell& other) 
{
    if (this != &other) 
        _client = other._client;
    return *this;
}

BonusShell::~BonusShell() {}

static std::string getHistoryPath_() 
{
    return ".taskmaster_bonus_history";
}


int BonusShell::run(const std::string& socketPath) 
{
    std::cout << "Taskmaster bonus client. Type 'quit' to leave.\n";
    std::string historyPath = getHistoryPath_();
    read_history(historyPath.c_str()); 

    while (true)
    {
        char* raw = readline("taskmaster_bonusctl> ");

        if (!raw) {
            std::cout << "\n";
            break;
        }

        std::string line(raw);
        free(raw);

        if (line.empty())
            continue;

        if (line == "quit")
            break;

        add_history(line.c_str());
        int rc = _client.sendCommand(socketPath, line);

        if (rc != 0) {
            std::cerr << "taskmaster_bonus: daemon not reachable.\n"
                         "Start it first with:\n"
                         "  ./taskmaster_bonus --server [config.yaml]\n";
        }
    }
    write_history(historyPath.c_str());
    std::cout << "Exiting Taskmaster client.\n";
    return 0;
}