/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IPCServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 04:18:37 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/06 04:45:20 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/IPCServer.hpp"

#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <cerrno>

static int setNonBlocking_(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

IPCServer::IPCServer(const std::string& sockPath, Handler h)
: sockPath_(sockPath), handler_(std::move(h)) {
    setupSocket_();
}

IPCServer::~IPCServer() {
    cleanup_();
}

void IPCServer::setupSocket_() {
    // Nettoyer un socket précédent
    ::unlink(sockPath_.c_str());

    listenFd_ = ::socket(AF_UNIX, SOCK_STREAM, 0);
    if (listenFd_ < 0) throw std::runtime_error("socket() failed");

    sockaddr_un addr; std::memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    std::snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", sockPath_.c_str());

    if (::bind(listenFd_, (sockaddr*)&addr, sizeof(addr)) < 0) {
        int e = errno; ::close(listenFd_); listenFd_ = -1;
        throw std::runtime_error(std::string("bind() failed: ") + std::strerror(e));
    }

    // Restriction droits: seul l'utilisateur courant
    ::chmod(sockPath_.c_str(), 0600);

    if (::listen(listenFd_, 16) < 0) {
        int e = errno; ::close(listenFd_); listenFd_ = -1;
        throw std::runtime_error(std::string("listen() failed: ") + std::strerror(e));
    }
    setNonBlocking_(listenFd_);
}

void IPCServer::cleanup_() {
    if (listenFd_ >= 0) { ::close(listenFd_); listenFd_ = -1; }
    if (!sockPath_.empty()) ::unlink(sockPath_.c_str());
}

void IPCServer::pollOnce() {
    if (listenFd_ < 0) return;

    sockaddr_un cli; socklen_t len = sizeof(cli);
    int fd = ::accept(listenFd_, (sockaddr*)&cli, &len);
    if (fd < 0) {
        return; // rien à accepter (non bloquant)
    }

    // Lire une commande (jusqu'au '\n')
    std::string line;
    char ch;
    while (true) {
        ssize_t n = ::read(fd, &ch, 1);
        if (n == 1) {
            if (ch == '\n') break;
            line.push_back(ch);
        } else {
            if (n == 0) break; // EOF
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            break; // erreur
        }
    }

    // Appeler le handler et renvoyer les lignes + sentinelle "."
    std::vector<std::string> out;
    try {
        out = handler_(line);
    } catch (...) {
        out = {"ERR internal handler error"};
    }

    for (const auto& s : out) {
        (void)::write(fd, s.c_str(), s.size());
        (void)::write(fd, "\n", 1);
    }
    (void)::write(fd, ".\n", 2);
    ::close(fd);
}
