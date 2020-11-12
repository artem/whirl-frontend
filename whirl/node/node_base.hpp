#pragma once

#include <whirl/node/node.hpp>
#include <whirl/node/services.hpp>
#include <whirl/node/peer_base.hpp>

#include <whirl/rpc/use/channel.hpp>

namespace whirl {

// Peer + Main routine

class NodeBase : public INode, public PeerBase {
 public:
  NodeBase(NodeServices services) : PeerBase(std::move(services)) {
  }

  void Start() override;

 private:
 protected:
  // Override this methods

  virtual void RegisterRPCServices(const rpc::IServerPtr& /*rpc_server*/) {
  }

  virtual void MainThread() {
  }

 private:
  // Main fiber routine
  void Main();
  void StartRPCServer();
};

}  // namespace whirl
