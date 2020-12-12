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
    return await::futures::WithInterrupt(
        std::move(f), After(timeout), TimeOutError());
  }

  // TrueTime

  const ITrueTimeServicePtr& TrueTime() const {
    return services_.true_time;
  }

  // Local storage backend

  const ILocalStorageBackendPtr& StorageBackend() const {
    return services_.storage_backend;
  }

  // Threads

  ThreadsRuntime& Threads() {
    return services_.threads;
  }

  void Spawn(ThreadRoutine routine) {
    Threads().Spawn(std::move(routine));
  }

  void SleepFor(Duration delay) {
    Threads().SleepFor(delay);
  }

  void Yield() {
    Threads().Yield();
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

  // Logger
  // Usage: NODE_LOG(format, values...)

  INodeLoggerPtr NodeLogger() const {
    return services_.logger;
  }

 private:
  NodeServices services_;
};

}  // namespace whirl
