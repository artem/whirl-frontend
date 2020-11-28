#pragma once

#include <whirl/matrix/world/actor.hpp>
#include <whirl/matrix/world/clock.hpp>
#include <whirl/matrix/process/heap.hpp>
#include <whirl/matrix/process/crash.hpp>
#include <whirl/matrix/process/step_queue.hpp>
#include <whirl/matrix/memory/allocator.hpp>

#include <whirl/matrix/log/logger.hpp>

namespace whirl {

class ProcessBase : public IActor {
 public:
  ProcessBase(const std::string name) : name_(name), logger_(name) {
  }

  // IActor

  // Context: local / global ?
  const std::string& Name() const override {
    return name_;
  }

  // Context: global
  bool IsRunnable() const override {
    auto g = heap_.Use();
    return !steps_.IsEmpty();
  }

  // Context: global
  TimePoint NextStepTime() const override {
    auto g = heap_.Use();
    return steps_.NextStepTime();
  }

  // Context: global
  void Step() override {
    auto g = heap_.Use();
    auto step = steps_.TakeNext();
    step();
  }

  void Shutdown() override {
    WHIRL_FMT_LOG("Bytes allocated: {}", heap_.BytesAllocated());

    {
      auto g = heap_.Use();
      steps_.Clear();
      ReleaseFibersOnCrash(heap_);
    }
    heap_.Reset();
  }

 protected:
  std::string name_;  // Allocated on global heap
  Logger logger_;

  mutable ProcessHeap heap_;
  StepQueue steps_;
};

}  // namespace whirl
