#include "detect_version_from_comments.hpp"
#include "svtoi.hpp"
#include <ctre.hpp>
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
int detect_version_from_comments(std::string_view config_contents) {
  // Search the contents line by line.
  using offset = std::string_view::size_type;
  offset start = 0;
  offset end;
  for (;;) {
    // Get string view for the line.
    end = config_contents.find('\n', start);
    if (end == std::string_view::npos) {
      end = config_contents.size();
    }
    auto line = config_contents.substr(start, end - start);

    // Search line for comment containing explicit clang-format versions.
    {
      auto [match, major] =
          ctre::match<R"(.*#.*clang-format\s+version\s+([0-9]+).*)">(line);
      if (match) {
        return svtoi(major);
      }
    }
    {
      auto [match, https, major] =
          ctre::match<R"(\s*#.*http(s?)://releases\.llvm\.org/([0-9]+).*)">(
              line);
      if (match) {
        return svtoi(major);
      }
    }

    // Continue to the next line.
    if (end == config_contents.size()) {
      break;
    } else {
      start = end + 1;
    }
  }

  return -1;
}
