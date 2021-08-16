#pragma once

#include <whirl/engines/matrix/adversary/strategy.hpp>

#include <whirl/engines/matrix/adversary/services/executor.hpp>
#include <whirl/engines/matrix/adversary/services/time.hpp>

#include <whirl/engines/matrix/memory/helpers.hpp>
#include <whirl/engines/matrix/process/step_queue.hpp>
#include <whirl/engines/matrix/server/server.hpp>
#include <whirl/engines/matrix/process/process_base.hpp>
#include <whirl/engines/matrix/process/threads.hpp>

#include <functional>
#include <deque>

namespace whirl::matrix::adversary {

//////////////////////////////////////////////////////////////////////

class Process : public ProcessBase {
 public:
  Process(Strategy strategy)
      : ProcessBase("Adversary"), strategy_(std::move(strategy)) {
  }

  void Start() override {
    auto g = heap_.Use();

    // Just throw runtime to this process heap
    ThreadsRuntime* runtime = MakeRuntime();

    runtime->Spawn([this, runtime]() {
      strategy_(*runtime);
    });
  }

 private:
  ThreadsRuntime* MakeRuntime() {
    return new ThreadsRuntime{MakeExecutor(), MakeTimeService()};
  }

  IExecutorPtr MakeExecutor() {
    return std::make_shared<Executor>(steps_);
  }

  ITimeService* MakeTimeService() {
    return MakeStaticLikeObject<TimeService>(steps_);
  }

 private:
  Strategy strategy_;
};

//////////////////////////////////////////////////////////////////////

}  // namespace whirl::matrix::adversary
