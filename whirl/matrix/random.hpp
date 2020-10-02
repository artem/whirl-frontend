#pragma once

#include <whirl/services/random.hpp>

// Impl
#include <whirl/matrix/global.hpp>

namespace whirl {

struct RandomService : public IRandomService {
  RandomUInt RandomNumber() override {
    return GlobalRandomNumber();
  }
};

}  // namespace whirl
