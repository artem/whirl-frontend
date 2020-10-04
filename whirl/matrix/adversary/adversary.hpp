#pragma once

#include <whirl/matrix/adversary/services/executor.hpp>
#include <whirl/matrix/adversary/services/time.hpp>

#include <whirl/matrix/common/event_queue.hpp>
#include <whirl/matrix/server/server.hpp>
#include <whirl/matrix/process/process_base.hpp>
#include <whirl/services/threads.hpp>
#include <whirl/matrix/world/world_view.hpp>
#include <whirl/matrix/world/global.hpp>

#include <functional>
#include <deque>

namespace whirl::adversary {

//////////////////////////////////////////////////////////////////////

using Strategy = std::function<void(ThreadsRuntime&, WorldView)>;

//////////////////////////////////////////////////////////////////////

class Process : public ProcessBase {
 public:
  Process(Strategy strategy)
      : ProcessBase("Adversary"), strategy_(std::move(strategy)) {
  }

  void SetWorld(WorldView world) {
    world_.emplace(world);
  }

  void Start() override {
    auto g = heap_.Use();

    // Just throw runtime to this process heap
    ThreadsRuntime* runtime = MakeRuntime();

    runtime->Spawn(
        [this, runtime, world = *world_]() { strategy_(*runtime, world); });
  }

 private:
  ThreadsRuntime* MakeRuntime() {
    return new ThreadsRuntime{MakeExecutor(), MakeTimeService()};
  }

  IExecutorPtr MakeExecutor() {
    return std::make_shared<Executor>(events_);
  }

  ITimeServicePtr MakeTimeService() {
    return std::make_shared<TimeService>(events_);
  }

 private:
  Strategy strategy_;
  std::optional<WorldView> world_;
};

//////////////////////////////////////////////////////////////////////

}  // namespace whirl::adversary
