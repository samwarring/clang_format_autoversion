#ifndef INCLUDED_DETECT_VERSION_FROM_COMMENTS_HPP
#define INCLUDED_DETECT_VERSION_FROM_COMMENTS_HPP

#include <string_view>

/**
 * @brief Searches the config for comments containing an explicit version.
 *
 * @details Stops searching on the first explicit version.
 *
 * @param config_contents Contents of .clang-format file
 *
 * @return If found, clang-format major version; if not found, a negative value.
 */
int detect_version_from_comments(std::string_view config_contents);

#endif