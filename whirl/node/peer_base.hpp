#pragma once

#include <whirl/node/node_methods_base.hpp>

#include <whirl/rpc/client.hpp>
#include <whirl/rpc/channel.hpp>

#include <vector>

namespace whirl {

class PeerBase : public NodeMethodsBase {
 public:
  PeerBase();

 protected:
  const rpc::IClientPtr& Client();

  size_t PeerCount() const;

  const std::vector<std::string>& Peers();

  rpc::IChannelPtr& PeerChannel(size_t index) const;
  rpc::IChannelPtr& SelfChannel() const;
  const std::string& PeerName(size_t index) const;

 private:
  rpc::IClientPtr MakeRpcClient() const;
  void LazyInit() const;

  void ConnectToPeers() const;
  rpc::IChannelPtr MakeChannel(const std::string& peer) const;

 private:
  mutable rpc::IClientPtr client_;
  mutable std::vector<rpc::IChannelPtr> channels_;
};

}  // namespace whirl
