#pragma once

#include <whirl/cereal/serialize.hpp>

// Support for wheels::Unit (std::monostate) serialization
#include <cereal/types/variant.hpp>

#include <wheels/support/unit.hpp>

namespace whirl {

inline std::string SerializedUnit() {
  return Serialize(wheels::Unit{});
}

}  // namespace whirl
