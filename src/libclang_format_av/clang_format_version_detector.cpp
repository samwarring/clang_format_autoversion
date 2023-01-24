#include "clang_format_version_detector.hpp"
#include <ctre.hpp>
#include <sstream>

void clang_format_version_detector::read_configuration(std::istream &stream) {
  std::string line;
  while (std::getline(stream, line)) {
    read_configuration_line(line);
  }
}

void clang_format_version_detector::read_configuration(std::string contents) {
  std::istringstream stream{std::move(contents)};
  read_configuration(stream);
}

void clang_format_version_detector::read_configuration_line(
    std::string_view line) {

  {
    auto [match, major_ver] =
        ctre::match<R"(\s*#.*clang-format\s+version\s+([0-9]+).*)">(line);
    if (match) {
      versions_.clear();
      versions_.push_back(std::stoi(major_ver.str()));
      return;
    }
  }
  {
    auto [match, https, major_ver] =
        ctre::match<R"(\s*#.*http(s?)://releases.llvm.org/([0-9]+).*)">(line);
    if (match) {
      versions_.clear();
      versions_.push_back(std::stoi(major_ver.str()));
    }
  }
}

int clang_format_version_detector::detected_version() const {
  return versions_.empty() ? -1 : versions_[0];
}