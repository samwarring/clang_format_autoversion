#include "detect_version.hpp"
#include <regex>
#include <sstream>

const std::regex PATTERN_EXPLICIT_VERSION{
    R"(^\s*#\s*clang-format\s+version\s+([0-9]+).*)", std::regex_constants::icase};
const std::regex PATTERN_URL{
    R"(^.*https://releases.llvm.org/([0-9]+)(\.[0-9]+)*.*$)", std::regex_constants::icase};

int detect_version(std::istream& configuration)
{
    std::string line;
    while(std::getline(configuration, line)) {
        std::smatch match;
        if (std::regex_match(line, match, PATTERN_EXPLICIT_VERSION)) {
            std::sub_match major_version_submatch = match[1];
            return std::stoi(major_version_submatch.str());
        }
        if (std::regex_match(line, match, PATTERN_URL)) {
            std::sub_match major_version_submatch = match[1];
            return std::stoi(major_version_submatch.str());
        }
    }
    return -1;
}