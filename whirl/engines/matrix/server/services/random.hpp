#pragma once

#include <whirl/services/random.hpp>

// Impl
#include <whirl/engines/matrix/world/global/random.hpp>

namespace whirl::matrix {

struct RandomService : public IRandomService {
  RandomUInt RandomNumber() override {
    return GlobalRandomNumber();
  }
};

}  // namespace whirl::matrix
