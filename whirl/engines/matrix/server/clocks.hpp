#pragma once

#include <whirl/engines/matrix/world/global/time.hpp>
#include <whirl/engines/matrix/world/global/behaviour.hpp>

namespace whirl::matrix {

// Local clocks

//////////////////////////////////////////////////////////////////////

/* Value \in [-99, +inf]
 * Effect:
 * 0 - world time rate
 * -75 - x0.25
 * -50 - x0.5
 * +100 - x2
 * +200 - x3
 */

class Drift {
 public:
  Drift(int value) : drift_(value) {
  }

  // For now
  // Real time duration -> user duration
  Duration Elapsed(Duration real) const {
    return (real * (100 + drift_)) / 100;
  }

  // For sleeps/timeouts
  // User duration -> real time duration
  Duration SleepOrTimeout(Duration user) const {
    return (user * 100) / (100 + drift_);
  }

 private:
  int drift_;
};

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

 private:
  static Duration InitLocalClockOffset() {
    return GetWorldBehaviour()->InitWallClockOffset();
  }

 private:
  Duration offset_{0};
};

//////////////////////////////////////////////////////////////////////

class MonotonicClock {
 public:
  MonotonicClock() : drift_(GetWorldBehaviour()->InitClockDrift()) {
    Reset();
  }

  void Reset() {
    init_ = GetWorldBehaviour()->ResetMonotonicClock();
    last_reset_ = GlobalNow();
  }

  TimePoint Now() const {
    return drift_.Elapsed(ElapsedSinceLastReset()) + init_;
  }

  // For timeouts and sleeps
  Duration SleepOrTimeout(Duration d) const {
    return drift_.SleepOrTimeout(d);
  }

 private:
  // Global time
  Duration ElapsedSinceLastReset() const {
    return GlobalNow() - last_reset_;
  }

 private:
  Drift drift_;
  TimePoint last_reset_;
  TimePoint init_;
};

}  // namespace whirl::matrix
