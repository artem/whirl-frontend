#pragma once

#include <whirl/time.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class Time {
 public:
  Time() = default;

  // Non-copyable
  Time(const Time&) = delete;
  Time& operator=(const Time&) = delete;

  void AdvanceTo(TimePoint future) {
    WHEELS_VERIFY(future >= now_, "Cannot move world time backward: now = "
                                      << now_ << " -> " << future);
    now_ = future;
  }

  TimePoint Now() const {
    return now_;
  }

 private:
  TimePoint now_{0};
};

}  // namespace whirl::matrix
