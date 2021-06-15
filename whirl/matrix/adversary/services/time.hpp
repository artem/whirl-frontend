#pragma once

#include <whirl/services/time.hpp>

#include <whirl/time.hpp>
#include <whirl/matrix/world/time.hpp>
#include <whirl/matrix/process/step_queue.hpp>

#include <await/futures/helpers.hpp>

namespace whirl::adversary {

using namespace await::futures;

class TimeService : public ITimeService {
 public:
  TimeService(StepQueue& events) : events_(events) {
  }

  // Adversary has access to global time

  TimePoint WallTimeNow() override {
    return GlobalNow();
  }

  TimePoint MonotonicNow() override {
    return GlobalNow();
  }

  Future<void> After(Duration d) override {
    auto tp = AfterWorldTime(d);

    auto [f, p] = MakeContract<void>();
    events_.Add(tp, [p = std::move(p)]() mutable {
      std::move(p).Set();
    });
    return std::move(f);
  }

 private:
  TimePoint AfterWorldTime(Duration d) const {
    return GlobalNow() + d;
  }

 private:
  StepQueue& events_;
};

}  // namespace whirl::adversary
