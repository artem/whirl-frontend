#pragma once

#include <whirl/time.hpp>

#include <whirl/helpers/priority_queue.hpp>

#include <wheels/support/function.hpp>

namespace whirl {

using Action = wheels::UniqueFunction<void()>;

class EventQueue {
 public:
  struct Event {
    TimePoint time;
    Action action;

    bool operator<(const Event& that) const {
      return time < that.time;
    }

    void operator()() {
      action();
    }
  };

  using Queue = std::multiset<Event>;

 public:
  void Add(TimePoint time, Action action) {
    events_.Insert({time, std::move(action)});
  }

  bool IsEmpty() const {
    return events_.IsEmpty();
  }

  TimePoint NextEventTime() const {
    return events_.Smallest().time;
  }

  Event TakeNext() {
    return events_.Extract();
  }

  void Clear() {
    events_.Clear();
  }

 private:
  PriorityQueue<Event> events_;
};

}  // namespace whirl
