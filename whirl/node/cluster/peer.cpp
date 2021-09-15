#include <whirl/node/cluster/peer.hpp>

#include <whirl/node/runtime/methods.hpp>

#include <whirl/node/rpc/retries.hpp>

namespace whirl::node::cluster {

Peer::Peer(const std::string& pool_name) : pool_name_(pool_name) {
  ConnectToPeers();
}

size_t Peer::NodeCount() const {
  return pool_.size();
}

std::vector<std::string> Peer::ListPeers(bool with_me) const {
  if (with_me) {
    return pool_;
  } else {
    return others_;
  }
}

const rpc::IChannelPtr& Peer::Channel(const std::string& peer) const {
  auto it = channels_.find(peer);
  return it->second;  // Or UB =(
}

const rpc::IChannelPtr& Peer::LoopBack() const {
  return Channel(rt::HostName());
}

rpc::IClientPtr Peer::MakeRpcClient() {
  return rpc::MakeClient(rt::NetTransport(), rt::Executor(),
                         rt::LoggerBackend());
}

void Peer::ConnectToPeers() {
  pool_ = rt::Discovery()->ListPool(pool_name_);

  auto client = MakeRpcClient();

  // others_ = pool_ \ {rt::HostName()}
  for (const auto& host : pool_) {
    if (host != rt::HostName()) {
      others_.push_back(host);
    }
  }

  for (const auto& host : pool_) {
    channels_.emplace(host, MakeRpcChannel(client, host));
  }
}

static rpc::BackoffParams RetriesBackoff() {
  return {50, 1000, 2};  // Magic
}

rpc::IChannelPtr Peer::MakeRpcChannel(rpc::IClientPtr client,
                                      const std::string& host) {
  auto transport = client->Dial(host);
  auto retries = rpc::WithRetries(std::move(transport), rt::TimeService(),
                                  rt::LoggerBackend(), RetriesBackoff());
  return retries;
}

}  // namespace whirl::node::cluster
