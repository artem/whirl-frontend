#include <whirl/node/runtime/shortcuts.hpp>

#include <await/fibers/core/fiber.hpp>
#include <await/fibers/static/services.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::node::rt {

// [lo, hi]
uint64_t RandomNumber(uint64_t lo, uint64_t hi) {
  WHEELS_VERIFY(lo <= hi, "lo > hi");
  return lo + RandomNumber(hi - lo + 1);
}

size_t RandomIndex(size_t size) {
  WHEELS_VERIFY(size > 0, "size == 0");
  return RandomNumber(/*bound=*/size);
}

void Go(await::fibers::FiberRoutine routine) {
  auto* f = await::fibers::CreateFiber(
      std::move(routine), FiberManager(), Executor(),
      await::fibers::BackgroundSupervisor(), await::context::NeverStop());

  f->Schedule();
}

}  // namespace whirl::node::rt
