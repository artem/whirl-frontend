#pragma once

#include <whirl/time.hpp>

#include <whirl/engines/matrix/helpers/priority_queue.hpp>

#include <wheels/support/function.hpp>

namespace whirl::matrix {

using Action = wheels::UniqueFunction<void()>;

class StepQueue {
 public:
  struct Step {
    TimePoint time;
    Action action;

    bool operator<(const Step& that) const {
      return time < that.time;
    }

    void operator()() {
      action();
    }
  };

 public:
  StepQueue() = default;

  void Add(TimePoint time, Action action) {
    steps_.Insert({time, std::move(action)});
  }

  bool IsEmpty() const {
    return steps_.IsEmpty();
  }

  TimePoint NextStepTime() const {
    return steps_.Smallest().time;
  }

  Step TakeNext() {
    return steps_.Extract();
  }

  void Clear() {
    steps_.Clear();
  }

  size_t Size() const {
    return steps_.Size();
  }

 private:
  PriorityQueue<Step> steps_;
};

}  // namespace whirl::matrix
