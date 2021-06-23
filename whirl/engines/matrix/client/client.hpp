#pragma once

#include <whirl/node/node.hpp>
#include <whirl/node/services.hpp>
#include <whirl/node/threads.hpp>

#include <whirl/rpc/channel.hpp>

#include <whirl/helpers/errors.hpp>

// TODO!
#include <whirl/engines/matrix/log/logger.hpp>

#include <await/futures/helpers.hpp>

#include <wheels/support/assert.hpp>

namespace whirl {

using await::futures::Future;

//////////////////////////////////////////////////////////////////////

class ClientBase : public INode {
 public:
  ClientBase(NodeServices services) : services_(std::move(services)) {
  }

  void Start() override {
    Threads().Spawn([this]() {
      Main();
    });
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
    channel_ = MakeClientChannel();
  }

 protected:
  // Me

  NodeId Id() const {
    return services_.config->Id();
  }

  // Cluster

  rpc::IChannelPtr Channel() {
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

  bool Either() const {
    return RandomNumber(2) == 1;
  }

  // Time

  TimePoint WallTimeNow() const {
    return services_.time_service->WallTimeNow();
  }

  TimePoint MonotonicNow() const {
    return services_.time_service->MonotonicNow();
  }

  // Timeouts

  Future<void> After(Duration d) {
    return TimeService()->After(d);
  }

  template <typename T>
  Future<T> WithTimeout(Future<T> f, Duration timeout) {
    return await::futures::WithInterrupt(std::move(f), After(timeout),
                                         TimeOutError());
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

  ThreadsRuntime Threads() {
    return {services_.executor, services_.time_service};
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

  const IRandomServicePtr& RandomService() const {
    return services_.random;
  }

  const IUidGeneratorPtr& UidsGenerator() const {
    return services_.uids;
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
  rpc::IChannelPtr channel_;

 protected:
  Logger logger_{"Client"};
};

//////////////////////////////////////////////////////////////////////

class ExactlyOnceClientBase : public ClientBase {
 public:
  ExactlyOnceClientBase(NodeServices services)
      : ClientBase(std::move(services)) {
  }

 protected:
  rpc::IChannelPtr MakeClientChannel() override;
};

}  // namespace whirl
