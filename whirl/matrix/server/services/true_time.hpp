#pragma once

#include <whirl/services/true_time.hpp>

#include <whirl/matrix/world/global.hpp>

namespace whirl {

class TrueTimeService : public ITrueTimeService {
 public:
  TTNow Now() override {
    static const Duration kEps = 5;

    // TODO: better

    // Access world clock
    TimePoint now = GlobalNow();

    return {now - kEps, now + kEps};
  }
};

}  // namespace whirl
