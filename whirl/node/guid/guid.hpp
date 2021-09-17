#pragma once

#include <string>

namespace whirl::node {

//////////////////////////////////////////////////////////////////////

using Guid = std::string;

//////////////////////////////////////////////////////////////////////

// Generates globally unique identifiers

struct IGuidGenerator {
  virtual ~IGuidGenerator() = default;

  virtual Guid Generate() = 0;
};

}  // namespace whirl::node
