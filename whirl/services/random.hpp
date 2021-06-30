#pragma once

#include <cstdlib>
#include <memory>

namespace whirl {

using RandomUInt = size_t;

struct IRandomService {
  virtual ~IRandomService() = default;

  virtual RandomUInt GenerateNumber() = 0;
};

using IRandomServicePtr = std::shared_ptr<IRandomService>;

}  // namespace whirl
