#pragma once

#include <await/fibers/core/api.hpp>
#include <await/fibers/sync/future.hpp>
#include <await/fibers/core/await.hpp>

#include <whirl/services/executor.hpp>
#include <whirl/services/time_service.hpp>

namespace whirl {

struct Runtime {
  await::executors::IExecutorPtr executor;
  ITimeServicePtr time_service;

  void Spawn(await::fibers::FiberRoutine routine) {
    await::fibers::Spawn(std::move(routine), executor);
  }

  void Yield() {
    await::fibers::Yield();
  }

  void SleepFor(Duration d) {
    await::fibers::Await(time_service->After(d)).ExpectOk();
  }
};

}  // namespace whirl
