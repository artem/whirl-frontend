#include <whirl/node/node_base.hpp>

#include <wheels/support/assert.hpp>

namespace whirl {

void NodeBase::Start() {
  Spawn([this]() { Main(); });
}

void NodeBase::StartRPCServer() {
  RPCServer()->Start();
}

// Main fiber routine
void NodeBase::Main() {
  await::fibers::self::SetName("main");

  StartRPCServer();
  RegisterRPCServices(RPCServer());
  MainThread();
}

}  // namespace whirl
