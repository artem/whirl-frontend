#include <whirl/node/node_base.hpp>

#include <whirl/runtime/methods.hpp>

#include <wheels/support/assert.hpp>

#include <whirl/rpc/server_impl.hpp>

namespace whirl::node {

void NodeBase::Start() {
  rt::Go([this]() {
    MainThread();
  });
}

void NodeBase::StartRpcServer() {
  server_ = std::make_shared<rpc::ServerImpl>(rt::NetTransport(), rt::Executor());
  server_->Start();
}

// Main fiber routine
void NodeBase::MainThread() {
  await::fibers::self::SetName("main");

  rt::PrintLine("Starting at T{}", rt::WallTimeNow());

  // TODO: Read dir from config
  rt::Database()->Open("/db");

  StartRpcServer();
  RegisterRPCServices(RpcServer());

  MainRoutine();
}

}  // namespace whirl::node
