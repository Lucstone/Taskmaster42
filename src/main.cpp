/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 06:36:30 by lnaidu            #+#    #+#             */
/*   Updated: 2025/10/15 11:10:46 by lnaidu           ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../include/ProcessManager.hpp"

int main() {
    std::cout << "Taskmaster started\n";

    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "❌ Échec du fork()\n";
        return EXIT_FAILURE;
    }
    else if (pid == 0) {
        // 👶 Code enfant
        std::cout << "➡️  Enfant : PID = " << getpid() << "\n";

        // (Optionnel) Remplacer le code enfant par une vraie commande
        const char* path = "/bin/sleep";
        char* const args[] = { (char*)"sleep", (char*)"3", nullptr };

        if (execvp(path, args) == -1) {
            perror("execvp");
            _exit(1);
        }
    }
    else {

        std::cout << "👨 Parent : PID = " << getpid() 
                  << " ; PID enfant = " << pid << "\n";

        int status = 0;
        pid_t ended = waitpid(pid, &status, 0); 
        if (ended == -1) {
            perror("waitpid");
        } else {
            if (WIFEXITED(status)) {
                int exitCode = WEXITSTATUS(status);
                std::cout << "✅ Enfant " << ended 
                          << " terminé normalement avec code " << exitCode << "\n";
            } else if (WIFSIGNALED(status)) {
                int sig = WTERMSIG(status);
                std::cout << "💥 Enfant " << ended 
                          << " tué par le signal " << sig << "\n";
            }
        }
    }

    return EXIT_SUCCESS;
}
