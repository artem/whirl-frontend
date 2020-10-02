#pragma once

#include <whirl/matrix/time.hpp>

#include <wheels/support/function.hpp>

#include <set>

namespace whirl {

//////////////////////////////////////////////////////////////////////

template <typename T>
class PriorityQueue {
 public:
  void Clear() {
    items_.clear();
  }

  bool IsEmpty() const {
    return items_.empty();
  }

  const T& Smallest() const {
    return *items_.begin();
  }

  T Extract() {
    return std::move(items_.extract(items_.begin()).value());
  }

  void Insert(T value) {
    items_.insert(std::move(value));
  }

 private:
  std::multiset<T> items_;
};

//////////////////////////////////////////////////////////////////////

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
