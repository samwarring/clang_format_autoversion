#include "detect_version_from_yaml.hpp"
#include <iostream>
#include <ryml/ryml.hpp>
#include <ryml/ryml_std.hpp>
#include <string>
#include <vector>

/**
 * @brief Validate a node in the YAML tree.
 *
 * @param node_path Path through the tree to the selected node. The selected
 * node is at the end of the list.
 */
static void check_node(const std::vector<ryml::NodeRef> &node_path) {
  for (auto node : node_path) {
    std::cout << "/ ";
    if (node.has_key()) {
      std::cout << "key=" << node.key() << ' ';
    }
    if (node.has_val()) {
      std::cout << "value=" << node.val() << ' ';
    }
    if (!node.has_key() && !node.has_val()) {
      if (node.is_doc()) {
        std::cout << "[doc] ";
      } else if (node.is_seq()) {
        std::cout << "[seq] ";
      } else if (node.is_map()) {
        std::cout << "[map] ";
      } else if (node.is_val()) {
        std::cout << "[val] ";
      } else {
        std::cout << "? ";
      }
    }
  }
  std::cout << std::endl;
}

static void traverse(std::vector<ryml::NodeRef> &node_path) {
  check_node(node_path);
  for (auto node : node_path.back().children()) {
    node_path.push_back(node);
    traverse(node_path);
    node_path.pop_back();
  }
}

std::vector<int> detect_version_from_yaml(std::string config_contents) {
  std::vector<int> versions;
  c4::substr config_contents_c4 = c4::to_substr(config_contents);
  ryml::Tree tree = ryml::parse_in_place(config_contents_c4);
  ryml::NodeRef root = tree.rootref();
  ryml::NodeRef config_node;
  if (root.is_map()) {
    // "bare" document
    config_node = root;
  } else if (root.is_seq() && root.first_child().is_doc() &&
             root.first_child().is_map()) {
    // contains list of documents. The first document is a map.
    config_node = root.first_child();
  } else {
    // neither
    return versions;
  }

  // Depth-first traversal through the tree
  std::vector<ryml::NodeRef> node_path;
  node_path.push_back(config_node);
  traverse(node_path);

  versions.push_back(13);
  return versions;
}
