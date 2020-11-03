#pragma once

#include <whirl/services/true_time.hpp>

#include <whirl/matrix/world/global.hpp>
#include <whirl/matrix/world/dice.hpp>

namespace whirl {

class TrueTimeService : public ITrueTimeService {
 public:
  TTNow Now() const override {
    // TODO: better

    // Access world clock
    TimePoint now = GlobalNow();

    auto u = TrueTimeUncertainty();

    auto earliest = (now > u) ? now - u : 0;
    auto latest = now + u;

    return {earliest, latest};
  }
};

}  // namespace whirl
