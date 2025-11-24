/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BonusShell.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 07:25:49 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/24 09:16:59 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "BonusShell.hpp"

#include <iostream>
#include <string>

BonusShell::BonusShell() {}
BonusShell::BonusShell(const BonusShell& other)
    : _client(other._client) {}
BonusShell& BonusShell::operator=(const BonusShell& other) {
    if (this != &other) _client = other._client;
    return *this;
}
BonusShell::~BonusShell() {}

int BonusShell::run(const std::string& socketPath)
{
    std::cout << "Taskmaster bonus client. Type 'quit' or 'exit' to leave.\n";

    std::string line;
    while (true) {
        std::cout << "taskmaster> " << std::flush;
        if (!std::getline(std::cin, line)) break;

        if (line == "quit" || line == "exit") break;
        if (line.empty()) continue;

        _client.sendCommand(socketPath, line);
    }

    return 0;
}
