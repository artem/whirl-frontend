#pragma once

#include <whirl/node/node.hpp>
#include <whirl/node/services.hpp>

// TODO!
#include <whirl/matrix/log/log.hpp>

#include <wheels/support/assert.hpp>

namespace whirl {

class ClientBase : public INode {
 public:
  ClientBase(NodeServices services, NodeConfig config)
      : services_(std::move(services)), config_(std::move(config)) {
  }

  void SetCluster(std::vector<std::string> nodes) override {
    nodes_ = std::move(nodes);
  }

  void Start() override {
    services_.threads.Spawn([this]() { Main(); });
  }

 private:
  void RandomPause() {
    Threads().SleepFor(RandomNumber(50));
  }

  rpc::IRPCChannelPtr MakeClientChannel();

  void ConnectToClusterNodes() {
    channel_ = rpc::TRPCChannel(MakeClientChannel());
  }

 protected:
  // Me

  NodeId Id() const {
    return config_.id;
  }

  // Context: Server
  const std::string& MyName() const {
    return config_.name;
  }

  // Cluster

  size_t NodeCount() const {
    return nodes_.size();
  }

  rpc::TRPCChannel& Channel() {
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
  void Main() {
    await::fibers::self::SetName("main");

    RandomPause();
    ConnectToClusterNodes();
    MainThread();
  }

 private:
  NodeServices services_;

  NodeConfig config_;

  std::vector<std::string> nodes_;
  rpc::TRPCChannel channel_;

 protected:
  Logger logger_{"Client"};
};

}  // namespace whirl
