#pragma once

#include <commute/rpc/client.hpp>
#include <commute/rpc/channel.hpp>

#include <vector>
#include <string>
#include <map>
#include <memory>

namespace whirl::node::cluster {

class Peer {
 public:
  Peer(const std::string& pool_name, uint64_t port);

  const std::string& PoolName() const {
    return pool_name_;
  }

  size_t NodeCount() const;

  std::vector<std::string> ListPeers(bool with_me = true) const;

  const commute::rpc::IChannelPtr& Channel(const std::string& peer) const;
  const commute::rpc::IChannelPtr& LoopBack() const;

 private:
  commute::rpc::IClientPtr MakeRpcClient();
  commute::rpc::IChannelPtr MakeRpcChannel(commute::rpc::IClientPtr client,
                                  const std::string& host, uint16_t port);
  void ConnectToPeers();

 private:
  const std::string pool_name_;
  uint16_t port_;

  std::vector<std::string> pool_;
  std::vector<std::string> others_;  // pool without this node

  std::map<std::string, commute::rpc::IChannelPtr> channels_;
};

}  // namespace whirl::node::cluster
