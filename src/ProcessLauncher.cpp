#include "ProcessLauncher.hpp"
#include "Utils.hpp"
#include "ConfigParser.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <vector>
#include <csignal>
#include <cstdlib>

pid_t ProcessLauncher::spawnOne(const ConfigParser& cfg) {
    pid_t pid = fork();
    if (pid < 0) { std::perror("fork"); return -1; }

    if (pid == 0) { // === enfant ===
        // détachement du terminal
        if (setsid() < 0) { std::perror("setsid"); _exit(127); }

        // répertoire de travail
        if (!cfg.getWorkingDir().empty() && chdir(cfg.getWorkingDir().c_str()) == -1) {
            std::perror("chdir"); _exit(127);
        }

        // umask
        umask(strToOctalUmask(cfg.getUmask()));

        // stdin => /dev/null
        int fd_in = open("/dev/null", O_RDONLY);
        if (fd_in == -1 || dup2(fd_in, STDIN_FILENO) == -1) { std::perror("stdin"); _exit(127); }
        if (fd_in > 2) close(fd_in);

        // stdout
        int fd_out = STDOUT_FILENO;
        if (!cfg.getStdoutPath().empty()) fd_out = open_for_redirect(cfg.getStdoutPath());
        if (fd_out == -1 || dup2(fd_out, STDOUT_FILENO) == -1) { std::perror("stdout"); _exit(127); }
        if (fd_out > 2) close(fd_out);

        // stderr
        int fd_err = STDERR_FILENO;
        if (!cfg.getStderrPath().empty()) fd_err = open_for_redirect(cfg.getStderrPath());
        if (fd_err == -1 || dup2(fd_err, STDERR_FILENO) == -1) { std::perror("stderr"); _exit(127); }
        if (fd_err > 2) close(fd_err);

        // variables d'environnement
        for (auto& kv : cfg.getEnv()) setenv(kv.first.c_str(), kv.second.c_str(), 1);

        // préparation des arguments
        std::vector<char*> argv;
        splitCmd(cfg.getCmd(), argv);
        if (argv.empty()) _exit(127);

        // exécution
        execvp(argv[0], argv.data());
        std::perror("execvp");
        _exit(127);
    }

    // === parent ===
    return pid;
}
