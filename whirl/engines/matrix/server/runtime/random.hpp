#pragma once

#include <whirl/services/random.hpp>

// Impl
#include <whirl/engines/matrix/world/global/random.hpp>

namespace whirl::matrix {

struct RandomGenerator : public IRandomService {
  uint64_t GenerateNumber(uint64_t bound) override {
    return GlobalRandomNumber() % bound;
  }
};

}  // namespace whirl::matrix
