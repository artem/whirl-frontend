#pragma once

#include <await/fibers/core/manager.hpp>

namespace whirl::matrix::process {

//////////////////////////////////////////////////////////////////////

struct FiberStack {
  char buf[128 * 1024];
};

//////////////////////////////////////////////////////////////////////

class FiberManager : public await::fibers::IFiberManager {
 public:
  await::fibers::FiberId GenerateId() override {
    return ++next_id_;
  }

  wheels::MutableMemView AcquireStack() override {
    FiberStack* stack = new FiberStack{};
    return {(char*)stack, sizeof(FiberStack)};
  }

  void ReleaseStack(wheels::MutableMemView view) override {
    FiberStack* stack = (FiberStack*)view.Begin();
    delete stack;
  }

 private:
  size_t next_id_{0};
};

}  // namespace whirl::matrix::process
