#pragma once

#include <whirl/services/random.hpp>

// Impl
#include <whirl/engines/matrix/world/global/random.hpp>

namespace whirl::matrix {

struct RandomGenerator : public IRandomService {
  RandomUInt GenerateNumber() override {
    return GlobalRandomNumber();
  }
};

}  // namespace whirl::matrix
