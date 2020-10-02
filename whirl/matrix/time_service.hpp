#pragma once

#include <whirl/services/time_service.hpp>

#include <whirl/matrix/clock.hpp>
#include <whirl/matrix/local_clocks.hpp>
#include <whirl/matrix/event_queue.hpp>
#include <whirl/matrix/log.hpp>

#include <await/futures/promise.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Time service impl

class TimeService : public ITimeService {
 public:
  TimeService(const WorldClock& world_clock, LocalWallTimeClock& local_clock,
              LocalMonotonicClock& local_monotonic_clock, EventQueue& events)
      : world_clock_(world_clock),
        local_clock_(local_clock),
        local_monotonic_clock_(local_monotonic_clock),
        events_(events) {
  }

  TimePoint WallTimeNow() override {
    return local_clock_.Now();
  }

  TimePoint MonotonicNow() override {
    return local_monotonic_clock_.Now();
  }

  Future<Unit> After(Duration d) override {
    auto tp = AfterWorldTime(d);

    auto [f, p] = await::futures::MakeContract<Unit>();
    WHIRL_LLOG("Add timer event at ts = " << tp);
    events_.Add(tp,
                [p = std::move(p)]() mutable { std::move(p).SetValue({}); });
    return std::move(f);
  }

 private:
  TimePoint AfterWorldTime(Duration d) const {
    return world_clock_.Now() + local_clock_.ShapeDuration(d);
  }

 private:
  const WorldClock& world_clock_;

  LocalWallTimeClock& local_clock_;
  LocalMonotonicClock& local_monotonic_clock_;

  EventQueue& events_;

  Logger logger_{"Time"};
};

}  // namespace whirl
