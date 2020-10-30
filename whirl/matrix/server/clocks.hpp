#pragma once

#include <whirl/matrix/world/global.hpp>
#include <whirl/matrix/world/dice.hpp>

namespace whirl {

// Local clocks

//////////////////////////////////////////////////////////////////////

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

  Duration ShapeDuration(Duration d) {
    return d;
  }

 private:
  Duration offset_{0};
};

//////////////////////////////////////////////////////////////////////

class MonotonicClock {
 public:
  MonotonicClock() {
    Reset();
  }

  void Reset() {
    start_ = GlobalNow();
    init_ = ResetMonotonicClock();
  }

  TimePoint Now() const {
    // TODO: drift
    return ElapsedSinceLastReset() + init_;
  }

 private:
  Duration ElapsedSinceLastReset() const {
    return GlobalNow() - start_;
  }

 private:
  TimePoint start_;
  Duration init_;
};

}  // namespace whirl
