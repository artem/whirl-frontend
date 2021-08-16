#pragma once

#include <whirl/time.hpp>

#include <whirl/engines/matrix/helpers/priority_queue.hpp>

#include <whirl/engines/matrix/memory/new.hpp>

#include <whirl/engines/matrix/world/global/time.hpp>

namespace whirl::matrix {

////////////////////////////////////////////////////////////////////////

struct ITask {
  virtual ~ITask() = default;
  virtual void Run() = 0;
};

////////////////////////////////////////////////////////////////////////

class TaskScheduler {
 public:
  struct ScheduledTask {
    TimePoint time;
    ITask* task;

    bool operator<(const ScheduledTask& that) const {
      return time < that.time;
    }

    void operator()() {
      task->Run();
    }
  };

 public:
  TaskScheduler() = default;

  void Schedule(TimePoint t, ITask* task) {
    GlobalAllocatorGuard g;
    queue_.Insert({t, task});
  }

  void ScheduleAsap(ITask* task) {
    GlobalAllocatorGuard g;
    auto asap = GlobalNow();
    Schedule(asap, task);
  }

  bool IsEmpty() const {
    return queue_.IsEmpty();
  }

  size_t QueueSize() const {
    return queue_.Size();
  }

  TimePoint NextTaskTime() const {
    return queue_.Smallest().time;
  }

  ITask* TakeNext() {
    return queue_.Extract().task;
  }

  void Reset() {
    queue_.Clear();
  }

  void Pause() {
    // Do nothing
  }

  void Resume(TimePoint at) {
    while (!queue_.IsEmpty() && queue_.Smallest().time < at) {
      auto next = queue_.Extract();
      queue_.Insert({at, next.task});
    }
  }

 private:
  PriorityQueue<ScheduledTask> queue_;
};

////////////////////////////////////////////////////////////////////////

// From user space

namespace detail {

template <typename F>
class AutomaticStorageTask : public ITask {
 public:
  AutomaticStorageTask(F&& f) : f_(std::move(f)) {
  }

  void Run() override {
    f_();
    delete this;
  }

 private:
  F f_;
};

}  // namespace detail

template <typename F>
void Schedule(TaskScheduler& scheduler, TimePoint t, F&& f) {
  ITask* task = new detail::AutomaticStorageTask(std::move(f));
  scheduler.Schedule(t, task);
}

}  // namespace whirl::matrix
