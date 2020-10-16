#pragma once

#include <whirl/time.hpp>

#include <wheels/support/assert.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class WorldClock {
 public:
  WorldClock() = default;

  // Non-copyable
  WorldClock(const WorldClock& that) = delete;
  WorldClock& operator=(const WorldClock& that) = delete;

  void MoveForwardTo(TimePoint tp) {
    WHEELS_VERIFY(tp >= now_, "Cannot move world clock backward");
    now_ = tp;
  }

  TimePoint Now() const {
    return now_;
  }

 private:
  TimePoint now_{0};
};

}  // namespace whirl
