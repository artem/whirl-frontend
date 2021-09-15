#pragma once

#include <whirl/node/rpc/client.hpp>
#include <whirl/node/rpc/channel.hpp>

#include <vector>
#include <string>
#include <map>
#include <memory>

namespace whirl::node::cluster {

//////////////////////////////////////////////////////////////////////

class Peer {
 public:
  Peer(const std::string& pool_name);

  const std::string& PoolName() const {
    return pool_name_;
  }

  size_t NodeCount() const;

  std::vector<std::string> ListPeers(bool with_me = true) const;

  const rpc::IChannelPtr& Channel(const std::string& peer) const;
  const rpc::IChannelPtr& LoopBack() const;

 private:
  rpc::IClientPtr MakeRpcClient();
  rpc::IChannelPtr MakeRpcChannel(rpc::IClientPtr client,
                                  const std::string& host);
  void ConnectToPeers();

 private:
  const std::string pool_name_;

  std::vector<std::string> pool_;
  std::vector<std::string> others_;  // pool without this node

  std::map<std::string, rpc::IChannelPtr> channels_;
};

//////////////////////////////////////////////////////////////////////

using PeerPtr = std::shared_ptr<Peer>;

inline PeerPtr MakePeer(const std::string& pool) {
  return std::make_shared<Peer>(pool);
}

}  // namespace whirl::node::cluster
