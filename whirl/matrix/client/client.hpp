#pragma once

#include <whirl/node/node.hpp>
#include <whirl/node/services.hpp>

#include <whirl/rpc/use/channel.hpp>

// TODO!
#include <whirl/matrix/log/logger.hpp>

#include <wheels/support/assert.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

class ClientBase : public INode {
 public:
  ClientBase(NodeServices services) : services_(std::move(services)) {
  }

  void Start() override {
    Threads().Spawn([this]() { Main(); });
  }

 protected:
  rpc::IChannelPtr MakeTransportChannel(const std::string& peer);
  virtual rpc::IChannelPtr MakeClientChannel();

  void DiscoverCluster() {
    cluster_ = services_.discovery->GetCluster();
  }

 private:
  void RandomPause() {
    Threads().SleepFor(RandomNumber(50));
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

  rpc::TChannel& Channel() {
    return channel_;
  }

  // Common functions

  // Randomness

  RandomUInt RandomNumber() const {
    return services_.random->RandomNumber();
  }

  RandomUInt RandomNumber(size_t bound) const {
    return RandomNumber() % bound;
  }

  RandomUInt RandomNumber(size_t lo, size_t hi) const {
    return lo + RandomNumber(hi - lo);
  }

  // Time

  TimePoint WallTimeNow() const {
    return services_.time_service->WallTimeNow();
  }

  TimePoint MonotonicNow() const {
    return services_.time_service->MonotonicNow();
  }

  // Threads

  void Spawn(ThreadRoutine routine) {
    Threads().Spawn(std::move(routine));
  }

  void Yield() {
    Threads().Yield();
  }

  void SleepFor(Duration d) {
    Threads().SleepFor(d);
  }

  // Uids

  Uid GenerateUid() const {
    return services_.uids->Generate();
  }

  // Local services

  ThreadsRuntime& Threads() {
    return services_.threads;
  }

  const ITimeServicePtr& TimeService() const {
    return services_.time_service;
  }

  const INodeLoggerPtr& NodeLogger() const {
    return services_.logger;
  }

  const rpc::IClientPtr& RPCClient() const {
    return services_.rpc_client;
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
  NodeServices services_;

  std::vector<std::string> cluster_;
  rpc::TChannel channel_;

 protected:
  Logger logger_{"Client"};
};

//////////////////////////////////////////////////////////////////////

class ExactlyOnceClientBase : public ClientBase {
 protected:
  rpc::IChannelPtr MakeClientChannel() override;
};

}  // namespace whirl
