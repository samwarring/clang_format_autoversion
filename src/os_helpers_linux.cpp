#include "os_helpers.hpp"
#include <spawn.h>
#include <iostream>
#include <cstring>
#include <sys/wait.h>

extern char** environ;

std::filesystem::path get_clang_format_binary_dir() {
    auto path = std::filesystem::canonical("/proc/self/exe");
    return path.parent_path();
}

int execute_child_process(std::vector<std::string>& argv) {
    // Build array of char* required for `posix_spawn`
    std::vector<char*> args;
    args.reserve(argv.size() + 1);
    for (auto& arg : argv) {
        args.push_back(arg.data());
    }
    args.push_back(NULL);

    // Start the child process.
    pid_t child_pid;
    auto ret = posix_spawn(&child_pid, argv[0].c_str(), NULL, NULL, &args[0], environ);
    if (ret != 0) {
        std::cerr << "error: posix_spawn failed (" << strerror(ret) << ')' << std::endl;
        std::exit(-1);
    }

    // Wait for process to exit.
    int exit_status;
    do {
        ret = waitpid(child_pid, &exit_status, 0);
        if (ret == -1) {
            std::cerr << "error: waitpid failed (" << strerror(ret) << ')' << std::endl;
            std::exit(-1);
        }
    } while(!WIFEXITED(exit_status) && !WIFSIGNALED(exit_status));

    // Return exit status.
    return WEXITSTATUS(exit_status);
}