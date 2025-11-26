/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BonusClient.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 07:22:35 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/26 04:16:20 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "BonusClient.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

BonusClient::BonusClient() {}

BonusClient::BonusClient(const BonusClient& other) { (void)other; }

BonusClient& BonusClient::operator=(const BonusClient& other) {
    (void)other;
    return *this;
}

BonusClient::~BonusClient() {}

int BonusClient::connectToServer_(const std::string& socketPath) const {
    int fd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    
    if (fd < 0) {
        std::perror("socket");
        return -1;
    }

    sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", socketPath.c_str());

    if (::connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        std::perror("connect");
        ::close(fd);
        return -1;
    }
    return fd;
}

int BonusClient::sendCommand(const std::string& socketPath, const std::string& commandLine) const {
    
    int         fd = connectToServer_(socketPath);
    
    if (fd < 0) {
        std::cerr << "taskmaster_bonus: daemon not reachable.\n"
        << "Start it first with:\n"
        << "  ./taskmaster_bonus --server [config.yaml]\n";
        return 1;
    }
    
    std::string cmd = commandLine;
    std::string line;
    char        ch;
    bool        gotDot = false;

    if (cmd.empty() || cmd.back() != '\n')
        cmd.push_back('\n');

    if (::write(fd, cmd.c_str(), cmd.size()) < 0) {
        std::perror("write");
        ::close(fd);
        return 1;
    }

    while (true) {
        ssize_t n = ::read(fd, &ch, 1);
        if (n == 1) {
            if (ch == '\n') {
                if (line == ".") {
                    gotDot = true;
                    break;
                }
                std::cout << line << std::endl;
                line.clear();
            } else {
                line.push_back(ch);
            }
        } else {
            break;
        }
    }
    ::close(fd);
    (void)gotDot; 
    return 0;
}