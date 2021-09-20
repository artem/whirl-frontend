#include <whirl/runtime/access.hpp>

#include <wheels/support/panic.hpp>

namespace whirl::node {

static EngineRuntime engine_runtime_;

static struct Initializer {
  Initializer() {
    engine_runtime_ = []() -> IRuntime& {
      WHEELS_PANIC("Runtime not set");
    };
  }
} initializer;

IRuntime& GetRuntime() {
  return engine_runtime_();
}

void SetupRuntime(EngineRuntime getter) {
  engine_runtime_ = getter;
}

}  // namespace whirl::node
