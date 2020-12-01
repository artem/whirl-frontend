#pragma once

#include <whirl/node/node_methods_base.hpp>

#include <whirl/rpc/use/channel.hpp>

#include <vector>

namespace whirl {

class PeerBase : public NodeMethodsBase {
 public:
  PeerBase(NodeServices services) : NodeMethodsBase(services) {
  }

 protected:
  size_t PeerCount() const;
  rpc::TChannel& PeerChannel(size_t index) const;
  rpc::TChannel& SelfChannel() const;
  const std::string& PeerName(size_t index) const;

 private:
  void LazyInit() const;

  void ConnectToPeers() const;
  rpc::IChannelPtr MakeChannel(const std::string& peer) const;

 private:
  mutable std::vector<rpc::TChannel> channels_;
};

}  // namespace whirl
