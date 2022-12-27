#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <cctype>
#include <filesystem>
#include "os_helpers.hpp"

constexpr static std::string_view CONFIG_FILE_VERSION_DECLARATION{"# clang-format version "};


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


int main(int argc, char** argv) {
    // Get version from `.clang-format` if it exists.
    auto version = read_version_from_config_file();
    if (version.empty()) {
        version = DEFAULT_CLANG_FORMAT_VERSION;
    }

    // Get path to desired clang-format binary.
    std::string clang_format_binary_name = "clang-format-av-" + version + EXE_EXTENSION;
    auto binary_dir = get_clang_format_binary_dir();
    auto clang_format_binary = binary_dir / clang_format_binary_name;
    if (!std::filesystem::is_regular_file(clang_format_binary)) {
        std::cerr << "Requested " << clang_format_binary << ", but it was not found.\n";
        return -1;
    }

    // Collect arguments into a vector.
    std::vector<std::string> args;
    args.reserve(argc);
    args.push_back(clang_format_binary.string());
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    // Run clang-format
    return execute_child_process(args);
}