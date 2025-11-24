/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BonusClient.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 18:00:00 by YOU               #+#    #+#             */
/*   Updated: 2025/11/24 09:31:16 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

class BonusClient {
public:
    BonusClient();
    BonusClient(const BonusClient& other);
    BonusClient& operator=(const BonusClient& other);
    ~BonusClient();

    int sendCommand(const std::string& socketPath,
                    const std::string& commandLine) const;

private:
    int connectToServer_(const std::string& socketPath) const;
};