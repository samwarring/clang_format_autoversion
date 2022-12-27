#ifndef INCLUDED_OS_HELPERS_HPP
#define INCLUDED_OS_HELPERS_HPP

#include <filesystem>
#include <string>
#include <vector>

// Gets directory containing `clang-format-av-XX[.exe]` binaries. These are assumed
// to reside in the same directory as the currently running `clang-format-av[.exe]`
// executable.
std::filesystem::path get_clang_format_binary_dir();


// Executes a child process, and waits for it to exit. Returns the exit status of
// the child process. Does not return if there was an error starting the child process.
int execute_child_process(const std::vector<std::string>& argv);

#endif