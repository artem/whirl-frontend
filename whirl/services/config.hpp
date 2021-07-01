#pragma once

#include <cstdlib>
#include <memory>

namespace whirl {

using NodeId = size_t;

struct IConfig {
  virtual ~IConfig() = default;

  // Small unique identifier
  virtual NodeId Id() const = 0;

  // TODO: attributes
};

using IConfigPtr = IConfig*;

}  // namespace whirl
