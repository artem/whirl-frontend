#include <whirl/node/runtime/methods.hpp>

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

commute::rpc::IServerPtr MakeRpcServer(uint16_t port) {
  return std::make_shared<commute::rpc::ServerImpl>(
      std::to_string(port),
      NetTransport(), Executor(), FiberManager(), LoggerBackend());
}

commute::rpc::IClientPtr MakeRpcClient() {
  return commute::rpc::MakeClient(
      NetTransport(), Executor(), LoggerBackend());
}

}  // namespace whirl::node::rt