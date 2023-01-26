#ifndef INCLUDED_SVTOI_HPP
#define INCLUDED_SVTOI_HPP

#include <charconv>
#include <limits>
#include <string_view>

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

#endif