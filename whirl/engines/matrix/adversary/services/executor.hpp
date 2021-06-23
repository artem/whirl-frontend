#pragma once

#include <whirl/services/executor.hpp>

#include <whirl/time.hpp>
#include <whirl/engines/matrix/world/global/time.hpp>
#include <whirl/engines/matrix/process/step_queue.hpp>

namespace whirl::adversary {

class Executor : public IExecutor {
 public:
  Executor(StepQueue& events) : events_(events) {
  }

  void Execute(Task&& task) override {
    events_.Add(NextTick(), std::move(task));
  }

 private:
  TimePoint NextTick() {
    return GlobalNow() + 1;
  }

 private:
  StepQueue& events_;
};

}  // namespace whirl::adversary