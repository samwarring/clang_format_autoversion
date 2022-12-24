#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <cctype>
#include <filesystem>
#include <sstream>
#ifdef _WIN32
#include <windows.h>
#endif

constexpr static std::string_view CONFIG_FILE_VERSION_DECLARATION{"# clang-format version "};
constexpr static std::string_view DUMP_CONFIG_OPTION{"--dump-config"};


// Searches the `.clang-format` file in the current working directory for a comment
// containing the desired clang-format version. If no match, return an empty string.
std::string read_version_from_config_file() {
    std::string version;
    std::ifstream fin{".clang-format"};
    if (!fin) {
        return version;
    }
    std::string line;
    while(getline(fin, line)) {
        if (line.rfind(CONFIG_FILE_VERSION_DECLARATION, 0) == 0) {
            // Found line that begins with the declaration
            const char* ch = line.c_str() + CONFIG_FILE_VERSION_DECLARATION.size();
            while (*ch != '\0' && std::isdigit(*ch)) {
                version += *ch;
                ++ch;
            }
            if (!version.empty()) {
                return version;
            }
        }
    }
    return version;
}


// Gets the path to the desired clang-format binary for the provided version.
// Assume the binaries are located next to the current executable.
std::filesystem::path get_clang_format_binary_path(const std::string& version) {
    std::filesystem::path exe_path;
    std::string extension;
#ifdef _WIN32
    char buffer[2048];
    DWORD name_size = GetModuleFileNameA(NULL, buffer, 2048);
    std::string_view full_path{buffer, name_size};
    exe_path = full_path;
    extension = ".exe";
#else
    exe_path = std::filesystem::canonical("/proc/self/exe");
#endif
    return exe_path.parent_path() / ("clang-format-av-" + version + extension);
}


// Executes the process and returns its exit code.
int execute_child_process(const char* application, const std::vector<std::string> args) {
    int exit_status = -1;
    std::ostringstream command_line;
    command_line << application;
    for (auto arg : args) {
        command_line << " \"";
        
        // Escape double-quote (") if part of an argument.
        for (auto ch : arg) {
            if (ch == '\"') {
                command_line << "\\\"";
            }
            else {
                command_line << ch;
            }
        }

        command_line << "\"";
    }
    auto command_line_str = command_line.str();
#ifdef _WIN32
    PROCESS_INFORMATION proc_info{0};
    STARTUPINFOA startup_info{0};
    startup_info.cb = sizeof(startup_info);
    if (!CreateProcessA(
        NULL,
        command_line_str.data(),
        NULL,
        NULL,
        TRUE, // inherit handles
        0,
        NULL,
        NULL,
        &startup_info, // startupinfo ptr
        &proc_info // process information ptr
    )) {
        std::cerr << "CreateProcess failed (" << GetLastError() << ')' << std::endl;
        return exit_status;
    }
    WaitForSingleObject(proc_info.hProcess, INFINITE);
    DWORD dw_exit_status = 0;
    if (!GetExitCodeProcess(proc_info.hProcess, &dw_exit_status)) {
        std::cerr << "GetExitCodeProcess failed (" << GetLastError() << ')' << std::endl;
        return exit_status;
    }
    exit_status = static_cast<int>(dw_exit_status);
#else
    // TODO: Linux implementation
#endif
    return exit_status;
}


int main(int argc, char** argv) {
    // Get version from `.clang-format` if it exists.
    auto version = read_version_from_config_file();
    if (version.empty()) {
        version = DEFAULT_CLANG_FORMAT_VERSION;
    }

    // Get path to desired clang-format binary.
    auto clang_format_binary = get_clang_format_binary_path(version);
    if (!std::filesystem::is_regular_file(clang_format_binary)) {
        std::cerr << "Requested clang-format-av-" << version << ", but it was not found.\n";
        return 1;
    }

    // Collect arguments into a vector.
    std::vector<std::string> args;
    args.reserve(argc - 1);
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    return execute_child_process(clang_format_binary.string().c_str(), args);
}