#pragma once

#include <whirl/services/executor.hpp>

#include <whirl/engines/matrix/process/step_queue.hpp>
#include <whirl/engines/matrix/world/global/time.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class EventQueueExecutor : public IExecutor {
 public:
  EventQueueExecutor(StepQueue& events) : events_(events) {
  }

  void Execute(Task&& task) override {
    events_.Add(ChooseTaskTime(), std::move(task));
  }

 private:
  TimePoint ChooseTaskTime() const {
    return GlobalNow() + 1;
  }

 private:
  StepQueue& events_;
};

}  // namespace whirl::matrix
