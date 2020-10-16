#pragma once

#include <string>
#include <memory>

namespace whirl {

//////////////////////////////////////////////////////////////////////

using Uid = std::string;

//////////////////////////////////////////////////////////////////////

// Generates globally unique identifiers

struct IUidGenerator {
  virtual ~IUidGenerator() = default;

  virtual Uid Generate() = 0;
};

using IUidGeneratorPtr = std::shared_ptr<IUidGenerator>;

}  // namespace whirl
