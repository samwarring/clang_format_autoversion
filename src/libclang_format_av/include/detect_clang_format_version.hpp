#ifndef INCLUDED_DETECT_CLANG_FORMAT_VERSION_HPP
#define INCLUDED_DETECT_CLANG_FORMAT_VERSION_HPP

#include <string>
#include <vector>

/**
 * @brief Detect clang-format version from .clang-format config file.
 *
 * @details The config file may contain an explicit version as part of a
 * comment, in which case the version is extracted and returned as the
 * recommended version, even if the contents of the file would otherwise be
 * un-parseable by that version.
 *
 * @param config_contents contents of the config file.
 *
 * @return std::vector<int> List of clang-format major versions that are capable
 * of parsing this file. The first number in the list is the recommended
 * version.
 */
std::vector<int> detect_clang_format_version(std::string config_contents);

#endif