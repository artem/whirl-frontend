#pragma once

#include <whirl/node/node.hpp>

#include <whirl/rpc/server.hpp>

namespace whirl::node {

// Peer + Main routine

class NodeBase : public INode {
 public:
  void Start() override;

 private:
 protected:
  // Override this methods

  virtual void RegisterRPCServices(const rpc::IServerPtr& /*rpc_server*/) {
  }

  virtual void MainRoutine() {
  }

  const rpc::IServerPtr& RpcServer() {
    return server_;
  }

 private:
  // Main fiber routine
  void MainThread();

  void StartRpcServer();

 private:
  rpc::IServerPtr server_;
};

}  // namespace whirl::node
