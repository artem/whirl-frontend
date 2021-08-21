#pragma once

#include <await/fibers/core/manager.hpp>

#include <wheels/support/id.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

struct FiberStack {
  char buf[128 * 1024];
};

//////////////////////////////////////////////////////////////////////

class FiberManager : public await::fibers::IFiberManager {
 public:
  await::fibers::FiberId GenerateId() override {
    return ids_.NextId();
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
  wheels::IdGenerator ids_;
};

}  // namespace whirl::matrix