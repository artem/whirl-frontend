#pragma once

#include <whirl/node/cluster/list.hpp>

#include <whirl/node/config/config.hpp>

#include <commute/rpc/client.hpp>
#include <commute/rpc/channel.hpp>

#include <string>
#include <map>
#include <memory>

namespace whirl::node::cluster {

class Peer {
 private:
  class [[nodiscard]] Lister {
   public:
    Lister(const Peer* peer) : peer_(peer) {
    }

    // List all pool members including this node
    List WithMe() const {
      return peer_->ListImpl(/*with_me=*/true);
    }

    // List all pool members excluding this node
    List WithoutMe() const {
      return peer_->ListImpl(/*with_me=*/false);
    }

   private:
    const Peer* peer_;
  };

 public:
  explicit Peer(cfg::IConfig* config);

  friend class Lister;

  const std::string& PoolName() const {
    return pool_name_;
  }

  size_t NodeCount() const;

  Lister ListPeers() const {
    return Lister{this};
  }

  const commute::rpc::IChannelPtr& Channel(const std::string& peer) const;
  const commute::rpc::IChannelPtr& LoopBack() const;

 private:
  const List& ListImpl(bool with_me) const;

  commute::rpc::IClientPtr MakeRpcClient();
  commute::rpc::IChannelPtr MakeRpcChannel(commute::rpc::IClientPtr client,
                                           const std::string& host,
                                           cfg::IConfig* config);

  void ConnectToPeers(cfg::IConfig* config);

 private:
  const std::string pool_name_;
  uint16_t port_;

  List pool_;
  List others_;  // pool without this node

  std::map<std::string, commute::rpc::IChannelPtr> channels_;
};

}  // namespace whirl::node::cluster
