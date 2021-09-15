#include <whirl/runtime/methods.hpp>

#include <await/fibers/core/fiber.hpp>
#include <await/fibers/static/services.hpp>

#include <whirl/node/rpc/server_impl.hpp>
#include <whirl/node/rpc/client.hpp>

namespace whirl::node::rt {

void Go(await::fibers::FiberRoutine routine) {
  auto* f = await::fibers::CreateFiber(
      std::move(routine), FiberManager(), Executor(),
      await::fibers::BackgroundSupervisor(), await::context::NeverStop());

  f->Schedule();
}

rpc::IServerPtr MakeRpcServer() {
  return std::make_shared<rpc::ServerImpl>(NetTransport(), Executor(),
                                           FiberManager(), LogBackend());
}

rpc::IClientPtr MakeRpcClient() {
  return rpc::MakeClient(NetTransport(), Executor(), LogBackend());
}

}  // namespace whirl::node::rt