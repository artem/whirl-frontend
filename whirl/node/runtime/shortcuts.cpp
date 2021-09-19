#include <whirl/node/runtime/shortcuts.hpp>

#include <await/fibers/core/fiber.hpp>
#include <await/fibers/static/services.hpp>

#include <commute/rpc/server_impl.hpp>
#include <commute/rpc/client.hpp>

namespace whirl::node::rt {

void Go(await::fibers::FiberRoutine routine) {
  auto* f = await::fibers::CreateFiber(
      std::move(routine), FiberManager(), Executor(),
      await::fibers::BackgroundSupervisor(), await::context::NeverStop());

  f->Schedule();
}

}  // namespace whirl::node::rt
