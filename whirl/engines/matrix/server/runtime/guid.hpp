#pragma once

#include <whirl/node/guid/guid.hpp>

#include <whirl/engines/matrix/world/global/guids.hpp>

namespace whirl::matrix {

struct GuidGenerator : public node::IGuidGenerator {
  GuidGenerator() = default;

  node::Guid Generate() override {
    return GenerateGuid();
  }
};

}  // namespace whirl::matrix
