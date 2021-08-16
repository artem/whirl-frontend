#pragma once

#include <whirl/engines/matrix/world/global/time.hpp>
#include <whirl/engines/matrix/world/global/time_model.hpp>

namespace whirl::matrix {

class WallClock {
 public:
  WallClock() : offset_(InitLocalClockOffset()) {
  }

  void AdjustOffset() {
    offset_ = InitLocalClockOffset();
    // This action does not affect active timers:
    // they rely on monotonic clock
  }

  TimePoint Now() const {
    return GlobalNow() + offset_;
  }

 private:
  static Duration InitLocalClockOffset() {
    return GetTimeModel()->InitWallClockOffset();
  }

 private:
  Duration offset_{0};
};

}  // namespace whirl::matrix
