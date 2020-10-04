#pragma once

#include <whirl/services/executor.hpp>

#include <whirl/matrix/world/time.hpp>
#include <whirl/matrix/world/clock.hpp>
#include <whirl/matrix/common/event_queue.hpp>

namespace whirl::adversary {

class Executor : public IExecutor {
 public:
  Executor(EventQueue& events) : events_(events) {
  }

  void Execute(Task&& task) override {
    events_.Add(NextTick(), std::move(task));
  }

 private:
  TimePoint NextTick() {
    return GlobalNow() + 1;
  }

 private:
  EventQueue& events_;
};

}  // namespace whirl::adversary