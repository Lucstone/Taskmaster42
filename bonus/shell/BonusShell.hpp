/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BonusShell.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 18:00:00 by YOU               #+#    #+#             */
/*   Updated: 2025/11/24 09:16:30 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#pragma once

#include <string>
#include "../client/BonusClient.hpp"

class BonusShell {
public:
    BonusShell();
    BonusShell(const BonusShell& other);
    BonusShell& operator=(const BonusShell& other);
    ~BonusShell();

    int run(const std::string& socketPath);

private:
    BonusClient _client;
};
