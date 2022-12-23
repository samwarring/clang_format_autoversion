#include <iostream>
#include <boost/process.hpp>
#include <fstream>
#include <string>
#include <string_view>
#include <cctype>
#include <filesystem>


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
#ifdef _WIN32
#else
    exe_path = std::filesystem::canonical("/proc/self/exe");
#endif
    return exe_path.parent_path() / ("clang-format-" + version);
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
        std::cerr << "Requested clang-format-" << version << ", but it was not found.\n";
        return 1;
    }

    // Collect arguments into a vector.
    std::vector<std::string_view> args;
    args.reserve(argc - 1);
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    // Spawn the child process and forward command line arguments.
    return boost::process::system(clang_format_binary.string(), boost::process::args=args);
}