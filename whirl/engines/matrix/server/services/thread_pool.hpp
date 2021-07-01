#pragma once

#include <whirl/services/executor.hpp>

#include <whirl/engines/matrix/process/step_queue.hpp>
#include <whirl/engines/matrix/world/global/time.hpp>

#include <memory>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class ThreadPool {
  class Executor : public await::executors::IExecutor {
   public:
    Executor(ThreadPool* pool) : pool_(pool) {
    }

    void Execute(await::executors::Task&& task) override {
      pool_->Submit(std::move(task));
    }

   private:
    ThreadPool* pool_;
  };

 public:
  ThreadPool(StepQueue& steps)
      : steps_(steps),
        executor_(MakeExecutor()) {
  }

  void Submit(Task&& task) {
    steps_.Add(Schedule(), std::move(task));
  }

  const await::executors::IExecutorPtr& GetExecutor() {
    return executor_;
  }

 private:
  TimePoint Schedule() const {
    return GlobalNow() + 1;  // TODO: ITimeModel?
  }

  await::executors::IExecutorPtr MakeExecutor() {
    return std::make_shared<Executor>(this);
  }

 private:
  StepQueue& steps_;
  IExecutorPtr executor_;
};

}  // namespace whirl::matrix
