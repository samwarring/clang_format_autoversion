#ifndef INCLUDED_CLANG_FORMAT_VERSION_DETECTOR_HPP
#define INCLUDED_CLANG_FORMAT_VERSION_DETECTOR_HPP

#include <istream>
#include <string>
#include <string_view>
#include <vector>

/**
 * @brief Detects intended clang-format version from .clang-format configuration
 */
class clang_format_version_detector {
public:
  /**
   * @brief Reads .clang-format configuration file from input stream
   * @param stream Input stream (ifstream or istringstream)
   */
  void read_configuration(std::istream &stream);

  /**
   * @brief Reads .clang-format configuration file from raw file contents
   * @param contents Complete contents of the configuration file.
   */
  void read_configuration(std::string contents);

  /**
   * @brief Reads a single line from the .clang-format configuraiton file.
   * @param line Line from configuration file.
   */
  void read_configuration_line(std::string_view line);

  /**
   * @brief Gets the detected version after reading the configuration file.
   * @return clang-format major version number, or -1 if no applicable version.
   */
  int detected_version() const;

private:
  std::vector<int> versions_;
};

#endif