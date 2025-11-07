/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IPCServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 04:17:10 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/07 07:27:01 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <functional>
#include <vector>
#include <stdexcept>

class IPCServer {
public:
    // Handler: prend une commande (ligne SANS '\n'), renvoie des lignes de réponse (SANS '\n').
    using Handler = std::function<std::vector<std::string>(const std::string& line)>;

    // sockPath: ex. "/tmp/taskmaster.sock"
    IPCServer(const std::string& sockPath, Handler h);
    ~IPCServer();

    // À appeler régulièrement (non bloquant) dans la boucle principale.
    void                pollOnce();

    // Non copiable
    IPCServer(const IPCServer&) = delete;
    IPCServer& operator=(const IPCServer&) = delete;

private:
    std::string         _sockPath;
    int                 _listenFd;
    Handler             _handler;

    void                setupSocket_();
    void                cleanup_();
};
