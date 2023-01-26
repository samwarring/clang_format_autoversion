#include "detect_version.hpp"
#include "detect_version_from_comments.hpp"
#include "detect_version_from_yaml.hpp"
#include <string>
#include <vector>

std::vector<int> detect_version(std::string config_contents) {
  std::vector<int> versions;
  int explicit_version = detect_version_from_comments(config_contents);
  if (explicit_version > 0) {
    versions.push_back(explicit_version);
    return versions;
  }

  versions = detect_version_from_yaml(std::move(config_contents));
  return versions;
}