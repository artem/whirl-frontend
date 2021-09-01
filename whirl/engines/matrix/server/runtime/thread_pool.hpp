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
      : scheduler_(scheduler), executor_(this) {
  }

  void Submit(await::executors::TaskBase* user_task) {
    scheduler_.Schedule(ScheduleTask(), ConvertTask(user_task));
  }

  await::executors::IExecutor* GetExecutor() {
    return &executor_;
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

 private:
  process::Scheduler& scheduler_;
  Executor executor_;
};

}  // namespace whirl::matrix
