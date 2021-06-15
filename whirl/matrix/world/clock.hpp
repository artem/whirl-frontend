#pragma once

#include <whirl/time.hpp>

#include <wheels/support/assert.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class WorldClock {
 public:
  WorldClock() = default;

  // Non-copyable
  WorldClock(const WorldClock&) = delete;
  WorldClock& operator=(const WorldClock&) = delete;

  void MoveForwardTo(TimePoint future) {
    WHEELS_VERIFY(future >= now_, "Cannot move world clock backward: now = " << now_ << " -> " << future);
    now_ = future;
  }

  TimePoint Now() const {
    return now_;
  }

 private:
  TimePoint now_{0};
};

}  // namespace whirl
