#pragma once

#include <whirl/services/true_time.hpp>

#include <whirl/engines/matrix/world/global/time.hpp>
#include <whirl/engines/matrix/world/global/time_model.hpp>

namespace whirl::matrix {

class TrueTimeService : public ITrueTimeService {
 public:
  TTInterval Now() const override {
    // TODO: better

    // Access world clock
    TimePoint now = GlobalNow();

    auto u = GetTimeModel()->TrueTimeUncertainty();

    auto earliest = (now > u) ? now - u : 0;
    auto latest = now + u;

    return {earliest, latest};
  }
};

}  // namespace whirl::matrix
