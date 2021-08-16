#pragma once

#include <await/fibers/core/api.hpp>
#include <await/fibers/sync/future.hpp>
#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/thread_like.hpp>

#include <whirl/services/executor.hpp>
#include <whirl/services/time.hpp>

namespace whirl {

// TODO: remove

using ThreadRoutine = await::fibers::FiberRoutine;
using await::fibers::ThreadLike;

class ThreadsRuntime {
 public:
  ThreadsRuntime(IExecutorPtr e, ITimeService* t)
      : executor_(std::move(e)), time_service_(t) {
  }

  // TODO: Remove
  ThreadsRuntime() = default;

  void Spawn(ThreadRoutine routine) {
    await::fibers::Go(std::move(routine), executor_);
  }

  ThreadLike Thread(ThreadRoutine routine) {
    return ThreadLike{executor_, std::move(routine)};
  }

  void Yield() {
    await::fibers::self::Yield();
  }

  void SleepFor(Duration d) {
    await::fibers::Await(time_service_->After(d)).ExpectOk();
  }

 private:
  IExecutorPtr executor_;
  ITimeService* time_service_;
};

}  // namespace whirl
