#include <whirl/node/node_base.hpp>

#include <wheels/support/assert.hpp>

#include <whirl/rpc/server_impl.hpp>

namespace whirl::node {

void NodeBase::Start() {
  Go([this]() {
    MainThread();
  });
}

void NodeBase::StartRpcServer() {
  server_ = std::make_shared<rpc::ServerImpl>(NetTransport(), Executor());
  server_->Start();
}

// Main fiber routine
void NodeBase::MainThread() {
  await::fibers::self::SetName("main");

  PrintLine("Starting at T{}", WallTimeNow());

  // TODO: Read dir from config
  Database()->Open("/db");

  StartRpcServer();
  RegisterRPCServices(RpcServer());

  MainRoutine();
}

}  // namespace whirl::node
