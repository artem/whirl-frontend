#pragma once

namespace whirl::matrix::clocks {

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

}  // namespace whirl::matrix::clocks
