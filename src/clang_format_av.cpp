#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <cctype>
#include <filesystem>
#include "os_helpers.hpp"
#include "detect_version.hpp"

int main(int argc, char** argv) {
    // Get version from `.clang-format` if it exists.
    int version = -1;
    std::ifstream fin(".clang-format");
    if (fin) {
        version = detect_version(fin);
    }
    if (version == -1) {
        version = DEFAULT_CLANG_FORMAT_VERSION;
    }

    // Get path to desired clang-format binary.
    std::ostringstream clang_format_binary_name;
    clang_format_binary_name << "clang-format-av-" << version << EXE_EXTENSION;
    auto binary_dir = get_clang_format_binary_dir();
    auto clang_format_binary = binary_dir / clang_format_binary_name.str();
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