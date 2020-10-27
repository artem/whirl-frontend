#pragma once

#include <whirl/node/node.hpp>
#include <whirl/node/services.hpp>

#include <whirl/rpc/use/channel.hpp>

// TODO!
#include <whirl/matrix/log/logger.hpp>

#include <wheels/support/assert.hpp>

namespace whirl {

class ClientBase : public INode {
 public:
  ClientBase(NodeServices services) : services_(std::move(services)) {
  }

  void Start() override {
    Threads().Spawn([this]() { Main(); });
  }

 private:
  void RandomPause() {
    Threads().SleepFor(RandomNumber(50));
  }

  std::string NodeAddress(std::string server) const;
  rpc::IChannelPtr MakeClientChannel();

  void DiscoverCluster() {
    cluster_ = services_.discovery->GetCluster();
  }

  void ConnectToClusterNodes() {
    channel_ = rpc::TChannel(MakeClientChannel());
  }

 protected:
  // Me

  NodeId Id() const {
    return services_.config->Id();
  }

  // Cluster

  size_t NodeCount() const {
    return cluster_.size();
  }

  rpc::TChannel& Channel() {
    return channel_;
  }

  // Shortcuts for common functions

  RandomUInt RandomNumber() {
    return services_.random->RandomNumber();
  }

  RandomUInt RandomNumber(size_t bound) {
    return RandomNumber() % bound;
  }

  RandomUInt RandomNumber(size_t lo, size_t hi) {
    return lo + RandomNumber(hi - lo);
  }

  TimePoint WallTimeNow() {
    return services_.time_service->WallTimeNow();
  }

  TimePoint MonotonicNow() {
    return services_.time_service->MonotonicNow();
  }

  // Local services

  ThreadsRuntime& Threads() {
    return services_.threads;
  }

  const ITimeServicePtr& TimeService() {
    return services_.time_service;
  }

  const INodeLoggerPtr& NodeLogger() {
    return services_.logger;
  }

 protected:
  // Override this methods

  virtual void MainThread() {
  }

 private:
  void Main();

 private:
  NodeServices services_;

  std::vector<std::string> cluster_;
  rpc::TChannel channel_;

 protected:
  Logger logger_{"Client"};
};

}  // namespace whirl
