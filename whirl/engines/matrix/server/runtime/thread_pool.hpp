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

    void Execute(await::executors::Task&& task) override {
      pool_->Submit(std::move(task));
    }

   private:
    ThreadPool* pool_;
  };

 public:
  ThreadPool(process::Scheduler& scheduler)
      : scheduler_(scheduler),
        pool_executor_(MakeExecutor()) {
  }

  void Submit(await::executors::Task&& task) {
    scheduler_.Schedule(ScheduleTask(), ConvertTask(std::move(task)));
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
    TaskAdapter(await::executors::Task&& impl)
        : impl_(std::move(impl)) {
    }

    void Run() override {
      impl_();
      delete this;
    }
   private:
    await::executors::Task impl_;
  };

  process::ITask* ConvertTask(await::executors::Task&& task) {
    return new TaskAdapter(std::move(task));
  }

  await::executors::IExecutorPtr MakeExecutor() {
    return std::make_shared<Executor>(this);
  }

 private:
  process::Scheduler& scheduler_;
  await::executors::IExecutorPtr pool_executor_;
};

}  // namespace whirl::matrix
