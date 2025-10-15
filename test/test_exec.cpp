#include <iostream>
#include <unistd.h>
#include <vector>
#include <sys/wait.h>


// Cette fonction lance une commande via "cmd" (donc via le shell)
void runWithCmd(const std::string &cmd) {
    std::cout << "\n=== TEST AVEC CMD ===\n";
    pid_t pid = fork();

    if (pid == 0) {
        // Enfant : exécution via le shell
        std::cout << "[child] execvp(/bin/sh -c \"" << cmd << "\")\n";
        execl("/bin/sh", "sh", "-c", cmd.c_str(), (char*)nullptr);
        perror("exec failed");
        _exit(1);
    } else {
        // Parent : attend la fin
        waitpid(pid, nullptr, 0);
        std::cout << "[parent] Commande terminée.\n";
    }
}

// Cette fonction lance une commande via "args" (sans shell)
void runWithArgs(const std::vector<std::string> &args) {
    std::cout << "\n=== TEST AVEC ARGS ===\n";
    pid_t pid = fork();

    if (pid == 0) {
        // Enfant : exécution directe
        std::vector<char*> argv;
        for (const auto &a : args)
            argv.push_back(const_cast<char*>(a.c_str()));
        argv.push_back(nullptr);

        std::cout << "[child] execvp(" << argv[0] << ")\n";
        execvp(argv[0], argv.data());
        perror("exec failed");
        _exit(1);
    } else {
        // Parent : attend la fin
        waitpid(pid, nullptr, 0);
        std::cout << "[parent] Commande terminée.\n";
    }
}

int main() {
    // Exemple 1 : avec cmd (le shell interprète la ligne complète)
    runWithCmd("echo Hello from cmd && ls -l /home/lnaidu/Documents/Projet42");

    // Exemple 2 : avec args (pas de shell, chaque argument est séparé)
    runWithArgs({"/bin/echo", "Hello", "from", "args"});
    runWithArgs({"/bin/ls", "-l", "/home/lnaidu/Documents/Projet42"});

    return 0;
}
