#pragma once

#include <string>

namespace whirl::node::guids {

// Generates globally unique strings

struct IGuidGenerator {
  virtual ~IGuidGenerator() = default;

  virtual std::string Generate() = 0;
};

}  // namespace whirl::node::guids
