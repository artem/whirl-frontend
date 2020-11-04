#pragma once

#include <cstdlib>
#include <memory>

namespace whirl {

struct IConfig {
  virtual ~IConfig() = default;

  // Small unique identifier
  virtual size_t Id() const = 0;

  // TODO: attributes
};

using IConfigPtr = std::shared_ptr<IConfig>;

}  // namespace whirl
