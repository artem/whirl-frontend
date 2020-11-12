#pragma once

#include <whirl/services/random.hpp>

// Impl
#include <whirl/matrix/world/global/random.hpp>

namespace whirl {

struct RandomService : public IRandomService {
  RandomUInt RandomNumber() override {
    return GlobalRandomNumber();
  }
};

}  // namespace whirl
