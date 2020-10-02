#pragma once

#include <whirl/matrix/event_queue.hpp>
#include <whirl/matrix/server.hpp>
#include <whirl/matrix/process_base.hpp>
#include <whirl/matrix/runtime.hpp>
#include <whirl/matrix/world_view.hpp>
#include <whirl/matrix/global.hpp>

#include <functional>
#include <deque>

namespace whirl {

//////////////////////////////////////////////////////////////////////

using AdvStrategy = std::function<void(Runtime&, WorldView)>;

//////////////////////////////////////////////////////////////////////

class AdvExecutor : public IExecutor {
 public:
  AdvExecutor(const WorldClock& clock, EventQueue& events)
      : world_clock_(clock), events_(events) {
  }

  void Execute(Task&& task) override {
    events_.Add(NextTick(), std::move(task));
  }

 private:
  TimePoint NextTick() {
    return world_clock_.Now() + 1;
  }

 private:
  const WorldClock& world_clock_;
  EventQueue& events_;
};

//////////////////////////////////////////////////////////////////////

class AdvTimeService : public ITimeService {
 public:
  AdvTimeService(const WorldClock& world_clock, EventQueue& events)
      : world_clock_(world_clock), events_(events) {
  }

  // Adversary has access to global time

  TimePoint WallTimeNow() override {
    return world_clock_.Now();
  }

  TimePoint MonotonicNow() override {
    return world_clock_.Now();
  }

  Future<Unit> After(Duration d) override {
    auto tp = AfterWorldTime(d);

    auto [f, p] = MakeContract<Unit>();
    events_.Add(tp,
                [p = std::move(p)]() mutable { std::move(p).SetValue({}); });
    return std::move(f);
  }

 private:
  TimePoint AfterWorldTime(Duration d) const {
    return world_clock_.Now() + d;
  }

 private:
  const WorldClock& world_clock_;
  EventQueue& events_;
};

//////////////////////////////////////////////////////////////////////

class IProgram {};

class AdversaryProcess : public ProcessBase {
 public:
  AdversaryProcess(AdvStrategy strategy)
      : ProcessBase("Adversary"), strategy_(std::move(strategy)) {
  }

  void Start(WorldView world) {
    HeapScope guard(&heap_);
    Runtime* runtime = MakeRuntime();
    runtime->Spawn([this, runtime, world]() { strategy_(*runtime, world); });
  }

 private:
  Runtime* MakeRuntime() {
    // Just throw to process heap somewhere
    return new Runtime{MakeExecutor(), MakeTimeService()};
  }

  IExecutorPtr MakeExecutor() {
    return std::make_shared<AdvExecutor>(GetWorldClock(), events_);
  }

  ITimeServicePtr MakeTimeService() {
    return std::make_shared<AdvTimeService>(GetWorldClock(), events_);
  }

 private:
  AdvStrategy strategy_;
};

//////////////////////////////////////////////////////////////////////

}  // namespace whirl
