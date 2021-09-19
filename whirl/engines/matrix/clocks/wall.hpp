#pragma once

#include <whirl/node/time/wall_time.hpp>

#include <whirl/engines/matrix/world/global/time.hpp>
#include <whirl/engines/matrix/world/global/time_model.hpp>

namespace whirl::matrix::clocks {

class WallClock {
 public:
  WallClock() : offset_(InitLocalClockOffset()) {
  }

  void AdjustOffset() {
    offset_ = InitLocalClockOffset();
    // This action does not affect active timers:
    // they rely on monotonic clock
  }

  node::time::WallTime Now() const {
    return {GlobalNow() + offset_};
  }

 private:
  static Jiffies InitLocalClockOffset() {
    return TimeModel()->InitWallClockOffset();
  }

 private:
  Jiffies offset_{0};
};

}  // namespace whirl::matrix::clocks
