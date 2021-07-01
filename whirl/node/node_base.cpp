#include <whirl/node/node_base.hpp>

#include <wheels/support/assert.hpp>

#include <whirl/rpc/server_impl.hpp>

namespace whirl {

void NodeBase::Start() {
  Go([this]() {
    Main();
  });
}

void NodeBase::StartRpcServer() {
  server_ = std::make_shared<rpc::ServerImpl>(NetTransport(), Executor());
  server_->Start();
}

// Main fiber routine
void NodeBase::Main() {
  await::fibers::self::SetName("main");

  StartRpcServer();
  RegisterRPCServices(RpcServer());
  MainThread();
}

}  // namespace whirl
