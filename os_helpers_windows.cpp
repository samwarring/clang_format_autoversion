#include <sstream>
#include <iostream>
#include <windows.h>
#include "os_helpers.hpp"

const std::size_t PATH_BUFFER_SIZE = 1024;

std::filesystem::path get_clang_format_binary_dir()
{
    // GetFileNameA(NULL, ...) gets path to the current executable.
    char buffer[PATH_BUFFER_SIZE];
    DWORD dwPathSize = GetModuleFileNameA(NULL, buffer, PATH_BUFFER_SIZE);
    std::filesystem::path path{buffer, buffer + dwPathSize};
    return path.parent_path();
}


int execute_child_process(const std::vector<std::string>& argv)
{
    // Join the arguments into a single string. Each argument is enclosed in double-quotes
    // and double-quotes occuring in each argument are escaped.
    std::ostringstream sout;
    bool first = true;
    for (auto& arg : argv) {
        if (first) {
            sout << '\"';
            first = false;
        }
        else {
            // All but the first argument preceeded by space to separate it from the
            // previous argument.
            sout << " \"";
        }
        for (auto& ch : arg) {
            if (ch == '\"') {
                sout << "\\\"";
            } else {
                sout << ch;
            }
        }
        sout << '\"';
    }
    std::string sCommandLine = sout.str();

    // Start process with the Windows API
    PROCESS_INFORMATION pi;
    STARTUPINFOA si;
    ZeroMemory(&pi, sizeof(pi));
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    if (!CreateProcessA(NULL, sCommandLine.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        std::cerr << "error: CreateProcessA failed (" << GetLastError() << ')' << std::endl;
        std::exit(-1);
    }

    // Wait for process to exit.
    DWORD dwRet = WaitForSingleObject(pi.hProcess, INFINITE);
    switch (dwRet) {
    case WAIT_OBJECT_0:
        // Wait succeeded.
        break;
    case WAIT_ABANDONED:
        std::cerr << "error: WaitForSingleObject returned WAIT_ABANDONED" << std::endl;
        std::exit(-1);
    case WAIT_TIMEOUT:
        std::cerr << "error: WaitForSingleObject returned WAIT_TIMEOUT" << std::endl;
        std::exit(-1);
    case WAIT_FAILED:
        std::cerr << "error: WaitForSingleObject returned WAIT_FAILED (" << GetLastError() << ')' << std::endl;
        std::exit(-1);
    default:
        std::cerr << "error: WaitForSingleObject returned unknown error (" << dwRet << ')' << std::endl;
        std::exit(-1);
    }

    // Get exit status
    DWORD dwExitStatus;
    if (!GetExitCodeProcess(pi.hProcess, &dwExitStatus) ) {
        std::cerr << "error: GetExitCodeProcess failed (" << GetLastError << ')' << std::endl;
    }

    return static_cast<int>(dwExitStatus);
}