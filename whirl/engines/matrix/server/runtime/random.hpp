#pragma once

#include <whirl/node/random/service.hpp>

// Impl
#include <whirl/engines/matrix/world/global/random.hpp>

namespace whirl::matrix {

struct RandomGenerator : public node::IRandomService {
  uint64_t GenerateNumber(uint64_t bound) override {
    return GlobalRandomNumber() % bound;
  }
};

}  // namespace whirl::matrix
