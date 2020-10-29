#include <whirl/matrix/process/crash.hpp>

#include <await/fibers/core/fiber.hpp>
#include <await/fibers/core/guts.hpp>
#include <await/fibers/core/stack.hpp>

#include <vector>

namespace whirl {

void ReleaseFibersOnCrash(const ProcessHeap &heap) {
  auto& alive = await::fibers::AliveFibers();

  // Fibers from `heap`
  std::vector<await::fibers::Fiber*> lost;

  for (auto& fiber : alive) {
    if (heap.FromHere((char*)&fiber)) {
      lost.push_back(&fiber);
    }
  }

  for (auto* f : lost) {
    // Release off-heap resources
    await::fibers::ReleaseStack(std::move(f->GetStack()));
    // Unlink from global alive list
    f->Unlink();
  }
}

}  // namespace whirl