#pragma once

#include <whirl/services/time.hpp>

#include <whirl/matrix/world/clock.hpp>
#include <whirl/matrix/server/clocks.hpp>
#include <whirl/matrix/common/event_queue.hpp>
#include <whirl/matrix/log/log.hpp>

#include <await/futures/promise.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Time service impl

class TimeService : public ITimeService {
 public:
  TimeService(LocalWallTimeClock& local_clock,
              LocalMonotonicClock& local_monotonic_clock, EventQueue& events)
        : local_clock_(local_clock),
        local_monotonic_clock_(local_monotonic_clock),
        events_(events) {
  }

  TimePoint WallTimeNow() override {
    return local_clock_.Now();
  }

  TimePoint MonotonicNow() override {
    return local_monotonic_clock_.Now();
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
    return GlobalNow() + local_clock_.ShapeDuration(d);
  }

 private:
  LocalWallTimeClock& local_clock_;
  LocalMonotonicClock& local_monotonic_clock_;

  EventQueue& events_;

  Logger logger_{"Time"};
};

}  // namespace whirl
