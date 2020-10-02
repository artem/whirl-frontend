#pragma once

namespace whirl {

//////////////////////////////////////////////////////////////////////

class LocalWallTimeClock {
 public:
  LocalWallTimeClock(const WorldClock& world_clock)
      : world_clock_(world_clock) {
  }

  void Adjust() {
    // TODO: Timers?
  }

  TimePoint Now() {
    return world_clock_.Now() + offset_;
  }

  Duration ShapeDuration(Duration d) {
    return d;
  }

 private:
  const WorldClock& world_clock_;
  Duration offset_{0};
};

//////////////////////////////////////////////////////////////////////

class LocalMonotonicClock {
 public:
  LocalMonotonicClock(const WorldClock& world_clock)
      : world_clock_(world_clock) {
  }

  void Reset() {
    start_ = world_clock_.Now();
  }

  TimePoint Now() {
    // TODO: drift
    return world_clock_.Now() - start_;
  }

 private:
  TimePoint start_;
  const WorldClock& world_clock_;
};

}  // namespace whirl
