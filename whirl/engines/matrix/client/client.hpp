#pragma once

#include <whirl/node/node.hpp>
#include <whirl/engines/matrix/process/threads.hpp>
#include <whirl/runtime/methods.hpp>

#include <whirl/rpc/client.hpp>
#include <whirl/rpc/channel.hpp>

#include <whirl/logger/log.hpp>

#include <await/futures/helpers.hpp>

#include <wheels/support/assert.hpp>

namespace whirl::matrix {

//////////////////////////////////////////////////////////////////////

class ClientBase : public node::INode {
 public:
  void Start() override {
    node::rt::Go([this]() {
      MainThread();
    });
  }

 protected:
  virtual rpc::IChannelPtr MakeClientChannel();

  void DiscoverCluster() {
    cluster_ = node::rt::Dns()->GetPool("server");
  }

 private:
  void RandomPause() {
    node::rt::SleepFor(node::rt::RandomNumber(50));
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
    return node::rt::RandomNumber(2) == 1;
  }

  // Cluster

  const std::vector<std::string>& Cluster() const {
    return cluster_;
  }

 protected:
  // Override this methods

  virtual void MainRoutine() {
  }

 private:
  void MainThread();

 private:
  std::vector<std::string> cluster_;

  rpc::IClientPtr client_;
  rpc::IChannelPtr channel_;

 protected:
  Logger logger_{"Client"};
};

}  // namespace whirl::matrix
