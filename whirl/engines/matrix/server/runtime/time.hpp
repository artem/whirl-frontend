#pragma once

#include <whirl/node/time/time.hpp>

#include <whirl/engines/matrix/clocks/wall.hpp>
#include <whirl/engines/matrix/clocks/monotonic.hpp>
#include <whirl/engines/matrix/process/scheduler.hpp>

#include <await/futures/core/future.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

// Time service impl

class TimeService : public node::time::ITimeService {
 public:
  TimeService(clocks::WallClock& wall_clock,
              clocks::MonotonicClock& monotonic_clock,
              process::Scheduler& scheduler)
      : wall_clock_(wall_clock),
        monotonic_clock_(monotonic_clock),
        scheduler_(scheduler) {
  }

  node::time::WallTime WallTimeNow() override {
    return wall_clock_.Now();
  }

  node::time::MonotonicTime MonotonicNow() override {
    return monotonic_clock_.Now();
  }

  await::futures::Future<void> After(Jiffies d) override {
    auto after = AfterGlobalTime(d);

    auto [f, p] = await::futures::MakeContract<void>();

    auto cb = [timer_promise = std::move(p)]() mutable {
      std::move(timer_promise).Set();
    };
    Schedule(scheduler_, after, std::move(cb));
    return std::move(f);
  }

 private:
  TimePoint AfterGlobalTime(Jiffies d) const {
    return GlobalNow() + monotonic_clock_.SleepOrTimeout(d);
  }

 private:
  clocks::WallClock& wall_clock_;
  clocks::MonotonicClock& monotonic_clock_;

  process::Scheduler& scheduler_;
};

}  // namespace whirl::matrix
