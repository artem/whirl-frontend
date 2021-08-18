#pragma once

#include <whirl/rpc/client.hpp>
#include <whirl/rpc/channel.hpp>

#include <vector>
#include <string>
#include <map>
#include <memory>

namespace whirl::node {

//////////////////////////////////////////////////////////////////////

class Peer {
 public:
  Peer(const std::string& pool);

  size_t NodeCount() const;

  std::vector<std::string> ListPeers(bool with_me = true) const;

  const rpc::IChannelPtr& Channel(const std::string& peer) const;
  const rpc::IChannelPtr& SelfChannel() const;

 private:
  rpc::IClientPtr MakeRpcClient();
  void ConnectToPeers();
  rpc::IChannelPtr MakeChannel(const std::string& host);

 private:
  const std::string& pool_name_;

  rpc::IClientPtr client_;
  std::vector<std::string> pool_;
  std::vector<std::string> others_;  // cluster without this node
  std::map<std::string, rpc::IChannelPtr> channels_;
};

//////////////////////////////////////////////////////////////////////

using PeerPtr = std::shared_ptr<Peer>;

inline PeerPtr MakePeer(const std::string& pool) {
  return std::make_shared<Peer>(pool);
}

}  // namespace whirl::node
