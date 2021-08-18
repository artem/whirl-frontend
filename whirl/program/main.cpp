#include <whirl/program/main.hpp>

#include <whirl/runtime/methods.hpp>
#include <whirl/rpc/server_impl.hpp>

#include <await/futures/core/future.hpp>
#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/future.hpp>

namespace whirl::node {

void MainPrologue() {
  await::fibers::self::SetName("main");

  rt::PrintLine("Starting at T{}", rt::WallTimeNow());

  // TODO: Read dir from config
  rt::Database()->Open("/db");
}

rpc::IServerPtr MakeRPCServer() {
  return std::make_shared<rpc::ServerImpl>(rt::NetTransport(), rt::Executor());
}

static await::futures::Future<void> Never() {
  auto [never, _] = await::futures::MakeContract<void>();
  return std::move(never);
}

void BlockForever() {
  await::fibers::Await(Never()).ExpectOk();
}

}  // namespace whirl::node
