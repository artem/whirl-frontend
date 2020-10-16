#include <whirl/node/node_base.hpp>

// TODO
#include <whirl/matrix/channels/logging.hpp>
#include <whirl/matrix/channels/retries.hpp>

#include <wheels/support/assert.hpp>

namespace whirl {

void NodeBase::Start() {
  Threads().Spawn([this]() { Main(); });
}

void NodeBase::StartRPCServer() {
  services_.rpc_server.Start();
}

void NodeBase::DiscoverCluster() {
  cluster_ = services_.discovery->GetCluster();
}

rpc::IRPCChannelPtr NodeBase::MakeChannel(const std::string& peer_addr) {
  auto transport = services_.rpc_client.MakeChannel(peer_addr);
  auto log = MakeLoggingChannel(std::move(transport));
  auto retries = WithRetries(std::move(log), TimeService());
  return retries;
}

void NodeBase::ConnectToPeers() {
  for (const auto& peer_addr : cluster_) {
    channels_.push_back(MakeChannel(peer_addr));
  }
}

// Main fiber routine
void NodeBase::Main() {
  await::fibers::self::SetName("main");

  StartRPCServer();
  RegisterRPCMethods(services_.rpc_server);
  DiscoverCluster();
  ConnectToPeers();
  MainThread();
}

}  // namespace whirl
