#include <whirl/node/peer_base.hpp>

#include <whirl/rpc/retries.hpp>

namespace whirl::node {

PeerBase::PeerBase() {
}

size_t PeerBase::ClusterSize() const {
  LazyInit();
  return channels_.size();
}

std::vector<std::string> PeerBase::Peers(bool with_me) const {
  LazyInit();

  if (with_me) {
    return cluster_;
  } else {
    return peers_;
  }
}

rpc::IChannelPtr& PeerBase::PeerChannel(const std::string& peer) const {
  LazyInit();
  return channels_[peer];
}

rpc::IChannelPtr& PeerBase::SelfChannel() const {
  return PeerChannel(HostName());
}

rpc::IClientPtr PeerBase::MakeRpcClient() const {
  return rpc::MakeClient(NetTransport(), Executor());
}

void PeerBase::LazyInit() const {
  if (channels_.empty()) {
    client_ = MakeRpcClient();
    ConnectToPeers();
  }
}

void PeerBase::ConnectToPeers() const {
  cluster_ = DiscoverCluster();

  // peers = cluster \ {HostName()}
  for (const auto& host : cluster_) {
    if (host != HostName()) {
      peers_.push_back(host);
    }
  }

  for (const auto& host : cluster_) {
    channels_.emplace(host, MakeChannel(host));
  }
}

static rpc::BackoffParams RetriesBackoff() {
  return {50, 1000, 2};  // Magic
}

rpc::IChannelPtr PeerBase::MakeChannel(const std::string& peer) const {
  auto transport = client_->Dial(peer);
  auto retries =
      rpc::WithRetries(std::move(transport), TimeService(), RetriesBackoff());
  return retries;
}

}  // namespace whirl::node
