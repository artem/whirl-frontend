#include <whirl/node/node_base.hpp>

// TODO
#include <whirl/matrix/channels/logging.hpp>
#include <whirl/matrix/channels/retries.hpp>

namespace whirl {

rpc::IRPCChannelPtr NodeBase::MakeChannelTo(const std::string& peer) {
  auto transport = services_.rpc_client.MakeChannel(peer);
  auto log = MakeLoggingChannel(std::move(transport));
  auto retries = WithRetries(std::move(log), TimeService());
  return retries;
}

// Main fiber routine
void NodeBase::Main() {
  await::fibers::SetName("main");

  StartRPCServer();
  RegisterRPCMethods(services_.rpc_server);
  ConnectToPeers();
  MainThread();
}

}  // namespace whirl
