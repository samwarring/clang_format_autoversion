#include "detect_clang_format_version.hpp"
#include <algorithm>
#include <charconv>
#include <ctre.hpp>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

/**
 * @brief Parses integer from a string_view
 *
 * @param sv String view containing a valid integer.
 *
 * @return int parsed integer, or INT_MIN if failed to parse.
 */
int svtoi(std::string_view sv) {
  int value;
  auto result = std::from_chars(sv.data(), sv.data() + sv.size(), value);
  if (result.ptr != sv.data() + sv.size()) {
    value = std::numeric_limits<int>::min();
  }
  return value;
}

/**
 * @brief Searches the config for comments containing an explicit version.
 *
 * @details Stops searching on the first explicit version.
 *
 * @param config_contents Contents of .clang-format file
 *
 * @return If found, clang-format major version; if not found, a negative value.
 */
int search_comments_for_version(std::string_view config_contents) {
  // Search the contents line by line.
  using offset = std::string_view::size_type;
  offset start = 0;
  offset end;
  do {
    // Get string view for the line.
    end = config_contents.find('\n', start);
    auto line = config_contents.substr(start, end);

    // Search line for comment containing explicit clang-format versions.
    {
      auto [match, major] =
          ctre::match<R"(.*#.*clang-format\s+version\s+([0-9]+).*)">(
              config_contents);
      if (match) {
        return svtoi(major);
      }
    }
    {
      auto [match, https, major] =
          ctre::match<R"(\s*#.*http(s?)://releases\.llvm\.org/([0-9]+).*)">(
              config_contents);
      if (match) {
        return svtoi(major);
      }
    }

    // Continue to the next line.
    start = (end == std::string_view::npos ? end : end + 1);
  } while (start != std::string_view::npos);

  return -1;
}

std::vector<int> detect_clang_format_version(std::string_view config_contents) {
  std::vector<int> versions;
  int explicit_version = search_comments_for_version(config_contents);
  if (explicit_version > 0) {
    versions.push_back(explicit_version);
    return versions;
  }

  return versions;
}