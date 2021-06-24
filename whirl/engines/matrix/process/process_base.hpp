#pragma once

#include <whirl/engines/matrix/world/actor.hpp>
#include <whirl/engines/matrix/world/time.hpp>
#include <whirl/engines/matrix/process/heap.hpp>
#include <whirl/engines/matrix/process/crash.hpp>
#include <whirl/engines/matrix/process/step_queue.hpp>
#include <whirl/engines/matrix/memory/new.hpp>

#include <whirl/logger/log.hpp>

namespace whirl::matrix {

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
    WHIRL_LOG_INFO("Bytes allocated: {}", heap_.BytesAllocated());

    {
      auto g = heap_.Use();
      steps_.Clear();
      ReleaseFiberResourcesOnCrash(heap_);
    }
    heap_.Reset();
  }

 protected:
  std::string name_;  // Allocated on global heap
  Logger logger_;

  mutable ProcessHeap heap_;
  StepQueue steps_;
};

}  // namespace whirl::matrix
