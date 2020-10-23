#pragma once

#include <whirl/services/time.hpp>

#include <whirl/matrix/world/clock.hpp>
#include <whirl/matrix/server/clocks.hpp>
#include <whirl/matrix/common/event_queue.hpp>
#include <whirl/matrix/log/logger.hpp>

#include <await/futures/promise.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Time service impl

class TimeService : public ITimeService {
 public:
  TimeService(LocalWallClock& wall_time_clock,
              LocalMonotonicClock& monotonic_clock, EventQueue& events)
      : wall_time_clock_(wall_time_clock),
        monotonic_clock_(monotonic_clock),
        events_(events) {
  }

  TimePoint WallTimeNow() override {
    return wall_time_clock_.Now();
  }

  TimePoint MonotonicNow() override {
    return monotonic_clock_.Now();
  }

  Future<void> After(Duration d) override {
    auto tp = AfterWorldTime(d);

    auto [f, p] = await::futures::MakeContract<void>();
    WHIRL_LOG("Add timer event at ts = " << tp);
    events_.Add(tp, [p = std::move(p)]() mutable { std::move(p).Set(); });
    return std::move(f);
  }

 private:
  TimePoint AfterWorldTime(Duration d) const {
    return GlobalNow() + wall_time_clock_.ShapeDuration(d);
  }

 private:
  LocalWallClock& wall_time_clock_;
  LocalMonotonicClock& monotonic_clock_;

  EventQueue& events_;

  Logger logger_{"Time"};
};

}  // namespace whirl
