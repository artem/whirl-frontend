#include <whirl/node/peer_base.hpp>

#include <whirl/matrix/channels/retries.hpp>

namespace whirl {

size_t PeerBase::PeerCount() const {
  LazyInit();
  return channels_.size();
}

rpc::TChannel& PeerBase::PeerChannel(size_t index) const {
  LazyInit();
  return channels_.at(index);
}

const std::string& PeerBase::PeerName(size_t index) const {
  return PeerChannel(index).Peer();
}

void PeerBase::LazyInit() const {
  if (channels_.empty()) {
    ConnectToPeers();
  }
}

void PeerBase::ConnectToPeers() const {
  auto cluster = DiscoverCluster();
  for (const auto& peer : cluster) {
    channels_.push_back(MakeChannel(peer));
  }
}

rpc::IChannelPtr PeerBase::MakeChannel(const std::string& peer) const {
  auto transport = RPCClient()->MakeChannel(peer);
  auto retries = WithRetries(std::move(transport), TimeService());
  return retries;
}

}  // namespace whirl
