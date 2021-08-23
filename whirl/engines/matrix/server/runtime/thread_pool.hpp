#pragma once

#include <whirl/engines/matrix/process/scheduler.hpp>
#include <whirl/engines/matrix/world/global/time.hpp>

#include <await/executors/executor.hpp>

#include <memory>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class ThreadPool {
  class Executor : public await::executors::IExecutor {
   public:
    Executor(ThreadPool* pool) : pool_(pool) {
    }

    void Execute(await::executors::TaskBase* task) override {
      pool_->Submit(task);
    }

   private:
    ThreadPool* pool_;
  };

 public:
  ThreadPool(process::Scheduler& scheduler)
      : scheduler_(scheduler), pool_executor_(MakeExecutor()) {
  }

  void Submit(await::executors::TaskBase* user_task) {
    scheduler_.Schedule(ScheduleTask(), ConvertTask(user_task));
  }

  const await::executors::IExecutorPtr& GetExecutor() {
    return pool_executor_;
  }

 private:
  TimePoint ScheduleTask() const {
    return GlobalNow();  // TODO: ITimeModel?
  }

  class TaskAdapter : public process::ITask {
   public:
    TaskAdapter(await::executors::TaskBase* impl) : impl_(impl) {
    }

    void Run() override {
      impl_->Run();
      delete this;
    }

   private:
    await::executors::TaskBase* impl_;
  };

  process::ITask* ConvertTask(await::executors::TaskBase* user_task) {
    return new TaskAdapter(user_task);
  }

  await::executors::IExecutorPtr MakeExecutor() {
    return std::make_shared<Executor>(this);
  }

 private:
  process::Scheduler& scheduler_;
  await::executors::IExecutorPtr pool_executor_;
};

}  // namespace whirl::matrix