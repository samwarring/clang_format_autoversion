#ifndef INCLUDED_DETECT_VERSION_HPP
#define INCLUDED_DETECT_VERSION_HPP
#include <istream>

/**
 * @brief Detects clang-format version from the contents of a
 *        .clang-format configuration file.
 * 
 * @param configuration Contents of the configuraiton file. Can be
 *                      fstream or istringstream.
 * @return clang-format major version, or -1 if no version could
 *         be detected.
 */
int detect_version(std::istream& configuration);

#endif