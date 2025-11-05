/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 06:36:30 by lnaidu            #+#    #+#             */
/*   Updated: 2025/11/05 06:04:52 by lnaidu           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ProcessManager.hpp"

#include <unistd.h>

int main() {
    ProcessManager pm;
    pm.loadConfig("config.yaml");
    pm.startAutostartPrograms();

    // boucle de supervision
    for (;;) {
        pm.tick();
        // pm.printStatus(); // décommente pour voir passer l’état
        usleep(200000); // 200ms
    }
    return 0;
}