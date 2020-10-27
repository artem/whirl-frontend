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
  services_.rpc_server->Start(services_.config->RpcPort());
}

void NodeBase::DiscoverCluster() {
  cluster_ = services_.discovery->GetCluster();
}

std::string NodeBase::PeerAddress(std::string peer) const {
  return peer + ":" + services_.config->RpcPort();
}

rpc::IChannelPtr NodeBase::MakeChannel(const std::string& peer) {
  auto transport = services_.rpc_client.MakeChannel(PeerAddress(peer));
  auto log = MakeLoggingChannel(std::move(transport));
  auto retries = WithRetries(std::move(log), TimeService());
  return retries;
}

void NodeBase::ConnectToPeers() {
  for (const auto& peer : cluster_) {
    channels_.push_back(MakeChannel(peer));
  }
}

// Main fiber routine
void NodeBase::Main() {
  await::fibers::self::SetName("main");

  StartRPCServer();
  RegisterRPCServices(services_.rpc_server);
  DiscoverCluster();
  ConnectToPeers();
  MainThread();
}

}  // namespace whirl
