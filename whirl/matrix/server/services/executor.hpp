#pragma once

#include <whirl/services/executor.hpp>

#include <whirl/matrix/common/event_queue.hpp>
#include <whirl/matrix/world/global.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class EventQueueExecutor : public IExecutor {
 public:
  EventQueueExecutor(EventQueue& events) : events_(events) {
  }

  void Execute(Task&& task) override {
    events_.Add(ChooseTaskTime(), std::move(task));
  }

 private:
  TimePoint ChooseTaskTime() const {
    return GlobalNow() + 1;
  }

 private:
  EventQueue& events_;
};

}  // namespace whirl
