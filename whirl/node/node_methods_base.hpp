#pragma once

#include <whirl/node/services.hpp>

#include <whirl/helpers/errors.hpp>

#include <await/futures/helpers.hpp>

namespace whirl {

using await::futures::Future;

class NodeMethodsBase {
 public:
  NodeMethodsBase(NodeServices services) : services_(std::move(services)) {
  }

 protected:
  const NodeServices& ThisNodeServices() const {
    return services_;
  }

  // Me

  NodeId Id() const {
    return services_.config->Id();
  }

  // Randomness

  size_t RandomNumber() const {
    return services_.random->RandomNumber();
  }

  // [0, bound)
  size_t RandomNumber(size_t bound) const {
    return RandomNumber() % bound;
  }

  // [lo, hi)
  size_t RandomNumber(size_t lo, size_t hi) const {
    return lo + RandomNumber(hi - lo);
  }

  // Clocks

  const ITimeServicePtr& TimeService() const {
    return services_.time_service;
  }

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

  // TrueTime

  const ITrueTimeServicePtr& TrueTime() const {
    return services_.true_time;
  }

  // Local storage backend

  const IDatabasePtr& StorageBackend() const {
    return services_.database;
  }

  // Threads

  // TODO: remove
  ThreadsRuntime Threads() {
    return ThreadsRuntime{services_.executor, services_.time_service};
  }

  void Spawn(ThreadRoutine routine) {
    await::fibers::Spawn(std::move(routine), services_.executor);
  }

  void SleepFor(Duration delay) {
    Threads().SleepFor(delay);
  }

  void Yield() {
    Threads().Yield();
  }

  const await::executors::IExecutorPtr& GetExecutor() {
    return services_.executor;
  }

  // Uids

  Uid GenerateUid() const {
    return services_.uids->Generate();
  }

  // Cluster

  std::vector<std::string> DiscoverCluster() const {
    return services_.discovery->GetCluster();
  }

  // RPC

  const rpc::IServerPtr& RPCServer() const {
    return services_.rpc_server;
  }

  const rpc::IClientPtr& RPCClient() const {
    return services_.rpc_client;
  }

  // Local services

  const IRandomServicePtr& RandomService() const {
    return services_.random;
  }

  const IUidGeneratorPtr& UidsGenerator() const {
    return services_.uids;
  }

  // Logger
  // Usage: NODE_LOG(format, values...)

  INodeLoggerPtr NodeLogger() const {
    return services_.logger;
  }

 private:
  NodeServices services_;
};

}  // namespace whirl
