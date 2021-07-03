#pragma once

#include <whirl/node/node.hpp>
#include <whirl/node/runtime.hpp>
#include <whirl/engines/matrix/process/threads.hpp>
#include <whirl/node/node_methods_base.hpp>

#include <whirl/rpc/client.hpp>
#include <whirl/rpc/channel.hpp>

#include <whirl/logger/log.hpp>

#include <await/futures/helpers.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class ClientBase : public INode, public NodeMethodsBase {
 public:
  void Start() override {
    await::fibers::Go([this]() {
      Main();
    }, Executor());
  }

 protected:
  virtual rpc::IChannelPtr MakeClientChannel();

  void DiscoverCluster() {
    cluster_ = GetRuntime().DiscoveryService()->GetCluster();
  }

 private:
  void RandomPause() {
    SleepFor(RandomNumber(50));
  }

  rpc::IClientPtr MakeRpcClient();

  void ConnectToClusterNodes() {
    client_ = MakeRpcClient();
    channel_ = MakeClientChannel();
  }

 protected:
  // Cluster

  rpc::IChannelPtr Channel() {
    return channel_;
  }

  // Common functions

  bool Either() const {
    return RandomNumber(2) == 1;
  }

  // Cluster

  const std::vector<std::string>& Cluster() const {
    return cluster_;
  }

 protected:
  // Override this methods

  virtual void MainThread() {
  }

 private:
  void Main();

 private:
  std::vector<std::string> cluster_;

  rpc::IClientPtr client_;
  rpc::IChannelPtr channel_;

 protected:
  Logger logger_{"Client"};
};

}  // namespace whirl::matrix
