#pragma once

#include <whirl/node/id.hpp>

#include <string>

namespace whirl {

struct NodeConfig {
  NodeId id;
  std::string name;

  NodeConfig(NodeId _id) : id(_id) {
  }

  NodeConfig(NodeId _id, const std::string& _name) : id(_id), name(_name) {
  }
};

}  // namespace whirl
