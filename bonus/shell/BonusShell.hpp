/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BonusShell.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/24 18:00:00 by YOU               #+#    #+#             */
/*   Updated: 2025/11/26 04:22:25 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <string>
# include "../client/BonusClient.hpp"

class BonusShell {
private:
    BonusClient _client;

public:
    BonusShell();
    BonusShell(const BonusShell& other);
    BonusShell& operator=(const BonusShell& other);
    ~BonusShell();

    int run(const std::string& socketPath);
};
