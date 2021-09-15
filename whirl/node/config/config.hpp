#pragma once

#include <cstdlib>
#include <optional>
#include <string>
#include <memory>

namespace whirl::node {

using NodeId = size_t;

struct IConfig {
  virtual ~IConfig() = default;

  // Predefined
  virtual NodeId Id() const = 0;
  virtual const std::string& PoolName() const = 0;

  virtual std::optional<std::string> Get(const std::string& key) = 0;

  // TODO: attributes
};

}  // namespace whirl::node
