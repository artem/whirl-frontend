#pragma once

#include <whirl/rpc/client.hpp>
#include <whirl/rpc/channel.hpp>

#include <map>
#include <vector>

namespace whirl::node {

class PeerBase {
 public:
  PeerBase();

 protected:
  size_t ClusterSize() const;

  std::vector<std::string> Peers(bool with_me = true) const;

  rpc::IChannelPtr& PeerChannel(const std::string& peer) const;
  rpc::IChannelPtr& SelfChannel() const;

 private:
  rpc::IClientPtr MakeRpcClient() const;
  void LazyInit() const;

  void ConnectToPeers() const;
  rpc::IChannelPtr MakeChannel(const std::string& peer) const;

 private:
  // TODO
  mutable rpc::IClientPtr client_;
  mutable std::vector<std::string> cluster_;
  mutable std::vector<std::string> peers_;  // cluster without this node
  mutable std::map<std::string, rpc::IChannelPtr> channels_;
};

}  // namespace whirl::node
