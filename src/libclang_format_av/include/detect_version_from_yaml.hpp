#ifndef INCLUDED_DETECT_VERSION_FROM_YAML_HPP
#define INCLUDED_DETECT_VERSION_FROM_YAML_HPP

#include <string>
#include <vector>

/**
 * @brief Detects clang-format version of the config file.
 *
 * @details This detection method parses the YAML tree from the file and
 * compares each key/value to an embedded schema. If the config file contains a
 * key-value not known by clang-format version X, then the result will NOT
 * contain X.
 *
 * @param config_contents Contents of .clang-format file.
 *
 * @return List of clang-format major-versions that are able to parse this
 * config file. If the file is not valid YAML, or if it contains a key not known
 * by any version, then the result is an empty list.
 */
std::vector<int> detect_version_from_yaml(std::string config_contents);

#endif