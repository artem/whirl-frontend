#pragma once

#include <whirl/services/executor.hpp>

#include <whirl/engines/matrix/process/scheduler.hpp>
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
  ThreadPool(TaskScheduler& scheduler)
      : scheduler_(scheduler),
        pool_executor_(MakeExecutor()) {
  }

  void Submit(Task&& task) {
    scheduler_.Schedule(ScheduleTask(), ConvertTask(std::move(task)));
  }

  const await::executors::IExecutorPtr& GetExecutor() {
    return pool_executor_;
  }

 private:
  TimePoint ScheduleTask() const {
    return GlobalNow();  // TODO: ITimeModel?
  }

  class TaskAdapter : public ITask {
   public:
    TaskAdapter(Task&& impl)
        : impl_(std::move(impl)) {
    }

    void Run() override {
      impl_();
      delete this;
    }
   private:
    Task impl_;
  };

  ITask* ConvertTask(Task&& task) {
    return new TaskAdapter(std::move(task));
  }

  await::executors::IExecutorPtr MakeExecutor() {
    return std::make_shared<Executor>(this);
  }

 private:
  TaskScheduler& scheduler_;
  IExecutorPtr pool_executor_;
};

}  // namespace whirl::matrix
