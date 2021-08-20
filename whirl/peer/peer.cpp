#include <whirl/peer/peer.hpp>

#include <whirl/runtime/methods.hpp>

#include <whirl/rpc/retries.hpp>

namespace whirl::node {

Peer::Peer(const std::string& pool_name)
  : pool_name_(pool_name) {
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

const rpc::IChannelPtr& Peer::SelfChannel() const {
  return Channel(rt::HostName());
}

rpc::IClientPtr Peer::MakeRpcClient() {
  return rpc::MakeClient(rt::NetTransport(), rt::Executor());
}

void Peer::ConnectToPeers() {
  pool_ = rt::Dns()->ListPool(pool_name_);

  client_ = MakeRpcClient();

  // others_ = pool_ \ {rt::HostName()}
  for (const auto& host : pool_) {
    if (host != rt::HostName()) {
      others_.push_back(host);
    }
  }

  for (const auto& host : pool_) {
    channels_.emplace(host, MakeChannel(host));
  }
}

static rpc::BackoffParams RetriesBackoff() {
  return {50, 1000, 2};  // Magic
}

rpc::IChannelPtr Peer::MakeChannel(const std::string& host) {
  auto transport = client_->Dial(host);
  auto retries =
      rpc::WithRetries(std::move(transport), rt::TimeService(), RetriesBackoff());
  return retries;
}

}  // namespace whirl::node
