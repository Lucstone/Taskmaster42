/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IPCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 04:18:37 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/24 09:30:56 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IPCServer.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <stdexcept>

static int setNonBlocking_(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

IPCServer::IPCServer(const std::string& sockPath, Handler h)
: _sockPath(sockPath), _listenFd(-1), _handler(h) {
    setupSocket_();
}

IPCServer::IPCServer(const IPCServer& other)
: _sockPath(other._sockPath), _listenFd(-1), _handler(other._handler) {
    // On ne duplique pas le FD, on crée un nouveau socket si besoin
    setupSocket_();
}

IPCServer& IPCServer::operator=(const IPCServer& other) {
    if (this != &other) {
        cleanup_();
        _sockPath = other._sockPath;
        _handler  = other._handler;
        _listenFd = -1;
        setupSocket_();
    }
    return *this;
}

IPCServer::~IPCServer() {
    cleanup_();
}

void IPCServer::setupSocket_() {
    // Nettoyer un socket précédent
    ::unlink(_sockPath.c_str());

    _listenFd = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (_listenFd < 0)
        throw std::runtime_error("IPCServer: socket() failed");

    sockaddr_un addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", _sockPath.c_str());

    if (::bind(_listenFd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        int e = errno;
        ::close(_listenFd);
        _listenFd = -1;
        throw std::runtime_error(std::string("IPCServer: bind() failed: ") + std::strerror(e));
    }

    // Restriction droits: seul l'utilisateur courant
    ::chmod(_sockPath.c_str(), 0600);

    if (::listen(_listenFd, 16) < 0) {
        int e = errno;
        ::close(_listenFd);
        _listenFd = -1;
        throw std::runtime_error(std::string("IPCServer: listen() failed: ") + std::strerror(e));
    }
    setNonBlocking_(_listenFd);
}

void IPCServer::cleanup_() {
    if (_listenFd >= 0) {
        ::close(_listenFd);
        _listenFd = -1;
    }
    if (!_sockPath.empty())
        ::unlink(_sockPath.c_str());
}

void IPCServer::pollOnce() {
    if (_listenFd < 0)
        return;

    sockaddr_un cli;
    socklen_t   len = sizeof(cli);
    int         fd  = ::accept(_listenFd, (sockaddr*)&cli, &len);
    if (fd < 0) {
        // Rien à accepter (non bloquant)
        return;
    }

    // Lire une commande (jusqu'au '\n')
    std::string line;
    char        ch;
    while (true) {
        ssize_t n = ::read(fd, &ch, 1);
        if (n == 1) {
            if (ch == '\n')
                break;
            line.push_back(ch);
        } else {
            if (n == 0)       // EOF
                break;
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            break;            // erreur
        }
    }

    std::vector<std::string> out;
    try {
        out = _handler(line);
    } catch (...) {
        out.clear();
        out.push_back("ERR internal handler error");
    }

    // Envoyer chaque ligne + une sentinelle "."
    for (std::vector<std::string>::const_iterator it = out.begin();
         it != out.end(); ++it) {
        (void)::write(fd, it->c_str(), it->size());
        (void)::write(fd, "\n", 1);
    }
    (void)::write(fd, ".\n", 2);

    ::close(fd);
}