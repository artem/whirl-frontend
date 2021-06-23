#pragma once

#include <whirl/services/time.hpp>

#include <whirl/engines/matrix/server/clocks.hpp>
#include <whirl/engines/matrix/process/step_queue.hpp>
#include <whirl/engines/matrix/log/logger.hpp>

#include <await/futures/core/future.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Time service impl

class TimeService : public ITimeService {
 public:
  TimeService(WallClock& wall_clock, MonotonicClock& monotonic_clock,
              StepQueue& events)
      : wall_clock_(wall_clock),
        monotonic_clock_(monotonic_clock),
        events_(events) {
  }

  TimePoint WallTimeNow() override {
    return wall_clock_.Now();
  }

  TimePoint MonotonicNow() override {
    return monotonic_clock_.Now();
  }

  Future<void> After(Duration d) override {
    auto tp = AfterGlobalTime(d);

    auto [f, p] = await::futures::MakeContract<void>();
    events_.Add(tp, [p = std::move(p)]() mutable {
      std::move(p).Set();
    });
    return std::move(f);
  }

 private:
  TimePoint AfterGlobalTime(Duration d) const {
    return GlobalNow() + monotonic_clock_.SleepOrTimeout(d);
  }

 private:
  WallClock& wall_clock_;
  MonotonicClock& monotonic_clock_;

  StepQueue& events_;

  Logger logger_{"Time"};
};

}  // namespace whirl
