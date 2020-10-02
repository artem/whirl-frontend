#pragma once

#include <whirl/matrix/actor.hpp>
#include <whirl/matrix/clock.hpp>
#include <whirl/matrix/heap.hpp>
#include <whirl/matrix/event_queue.hpp>

#include <whirl/matrix/log.hpp>

namespace whirl {

class ProcessBase : public IActor {
 public:
  ProcessBase(const std::string name) : name_(name) {
  }

  const std::string& Name() const override {
    return name_;
  }

  bool IsRunnable() const override {
    HeapScope guard(&heap_);
    return !events_.IsEmpty();
  }

  TimePoint NextStepTime() override {
    HeapScope guard(&heap_);
    return events_.NextEventTime();
  }

  void MakeStep() override {
    HeapScope guard(&heap_);
    auto event = events_.TakeNext();
    event();
  }

  void Shutdown() override {
    WHIRL_LOG("Total bytes allocated: " << heap_.BytesAllocated());

    {
      HeapScope guard(&heap_);
      events_.Clear();
    }
    heap_.Reset();
  }

 protected:
  std::string name_;

  mutable Heap heap_;
  EventQueue events_;
};

}  // namespace whirl
