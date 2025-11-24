/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IPCServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 04:17:10 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/24 09:30:32 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <functional>

class IPCServer {
public:
    // Handler: prend une ligne de commande, renvoie des lignes de réponses
    using Handler = std::function<std::vector<std::string>(const std::string&)>;

    IPCServer(const std::string& sockPath, Handler h);
    IPCServer(const IPCServer& other);
    IPCServer& operator=(const IPCServer& other);
    ~IPCServer();

    // À appeler régulièrement dans la boucle du daemon (non bloquant).
    void pollOnce();

private:
    std::string _sockPath;
    int         _listenFd;
    Handler     _handler;

    void setupSocket_();
    void cleanup_();
};