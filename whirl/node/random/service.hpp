#pragma once

#include <cstdint>

namespace whirl::node::random {

struct IRandomService {
  virtual ~IRandomService() = default;

  // [0, bound)
  virtual uint64_t GenerateNumber(uint64_t bound) = 0;
};

}  // namespace whirl::node::random
