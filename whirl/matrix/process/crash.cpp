#include <whirl/matrix/process/crash.hpp>

#include <await/fibers/core/fiber.hpp>
#include <await/fibers/core/guts.hpp>
#include <await/fibers/core/stack.hpp>

#include <vector>

namespace whirl {

void ReleaseFiberResourcesOnCrash(const ProcessHeap& heap) {
  auto& alive = await::fibers::AliveFibers();

  // Fibers from `heap`
  std::vector<await::fibers::Fiber*> local;

  for (auto& fiber : alive) {
    if (heap.FromHere((char*)&fiber)) {
      local.push_back(&fiber);
    }
  }

  for (auto* fiber : local) {
    // Release off-heap resources
    await::fibers::ReleaseStack(std::move(fiber->GetStack()));
    // Unlink from global alive list
    fiber->Unlink();
  }
}

}  // namespace whirl