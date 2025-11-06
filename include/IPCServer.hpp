/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IPCServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 04:17:10 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/06 04:45:19 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <functional>
#include <vector>
#include <stdexcept>

class IPCServer {
    using Handler = std::function<std::vector<std::string>(const std::string& line)>;
    private:
        std::string sockPath_;
        int listenFd_ = -1;
        Handler handler_;

        void setupSocket_();
        void cleanup_();
    public:
    // Handler: prend une commande (ligne sans '\n'), renvoie des lignes de réponse (sans '\n').

    // sockPath: ex. "/tmp/taskmaster.sock"
    IPCServer(const std::string& sockPath, Handler h);
    ~IPCServer();

    // À appeler régulièrement (non bloquant) dans ta boucle principale.
    void pollOnce();

    // Non copiable
    IPCServer(const IPCServer&) = delete;
    IPCServer& operator=(const IPCServer&) = delete;

};
