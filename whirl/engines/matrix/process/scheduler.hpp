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
    TimePoint at_time;
    ITask* task;

    bool operator<(const ScheduledTask& that) const {
      return at_time < that.at_time;
    }

    void operator()() {
      task->Run();
    }
  };

 public:
  TaskScheduler() = default;

  void Schedule(TimePoint at, ITask* task) {
    GlobalAllocatorGuard g;
    ScheduleImpl(at, task);
  }

  void ScheduleAsap(ITask* task) {
    GlobalAllocatorGuard g;
    auto asap = GlobalNow();
    ScheduleImpl(asap, task);
  }

  bool IsEmpty() const {
    return queue_.IsEmpty();
  }

  size_t QueueSize() const {
    return queue_.Size();
  }

  TimePoint NextTaskTime() const {
    return queue_.Smallest().at_time;
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
    while (!queue_.IsEmpty() && queue_.Smallest().at_time < at) {
      auto next = queue_.Extract();
      queue_.Insert({at, next.task});
    }
  }

 private:
  void ScheduleImpl(TimePoint at, ITask* task) {
    queue_.Insert({at, task});
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
