#pragma once

#include <string>
#include <memory>

namespace whirl {

//////////////////////////////////////////////////////////////////////

using Guid = std::string;

//////////////////////////////////////////////////////////////////////

// Generates globally unique identifiers

struct IGuidGenerator {
  virtual ~IGuidGenerator() = default;

  virtual Guid Generate() = 0;
};

using IGuidGeneratorPtr = IGuidGenerator*;

}  // namespace whirl
