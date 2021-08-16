#include <whirl/node/peer_base.hpp>

#include <whirl/rpc/retries.hpp>

namespace whirl {

PeerBase::PeerBase() {
}

size_t PeerBase::PeerCount() const {
  LazyInit();
  return channels_.size();
}

rpc::IChannelPtr& PeerBase::PeerChannel(size_t index) const {
  LazyInit();
  return channels_.at(index);
}

rpc::IChannelPtr& PeerBase::SelfChannel() const {
  // TODO: more reliable impl
  size_t self_index = Id() - 1;
  return PeerChannel(self_index);
}

const std::string& PeerBase::PeerName(size_t index) const {
  return PeerChannel(index)->Peer();
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
  auto cluster = DiscoverCluster();
  for (const auto& peer : cluster) {
    channels_.push_back(MakeChannel(peer));
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

}  // namespace whirl
