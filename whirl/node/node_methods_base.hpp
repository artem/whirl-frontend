#pragma once

#include <whirl/node/runtime.hpp>

#include <await/fibers/core/api.hpp>
#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/future.hpp>

namespace whirl {

using await::futures::Future;

class NodeMethodsBase {
 protected:
  // Me

  NodeId Id() const {
    return GetRuntime().config->Id();
  }

  // Randomness

  const IRandomServicePtr& RandomService() const {
    return GetRuntime().random;
  }

  size_t RandomNumber() const {
    return RandomService()->RandomNumber();
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
    return GetRuntime().time_service;
  }

  TimePoint WallTimeNow() const {
    return TimeService()->WallTimeNow();
  }

  TimePoint MonotonicNow() const {
    return TimeService()->MonotonicNow();
  }

  // Timeouts

  Future<void> After(Duration d) {
    return TimeService()->After(d);
  }

  // TrueTime

  const ITrueTimeServicePtr& TrueTime() const {
    return GetRuntime().true_time;
  }

  // Local storage backend

  const IDatabasePtr& StorageBackend() const {
    return GetRuntime().database;
  }

  void Go(await::fibers::FiberRoutine routine) {
    await::fibers::Go(std::move(routine), Executor());
  }

  void SleepFor(Duration delay) {
    await::fibers::Await(After(delay)).ExpectOk();
  }

  void Yield() {
    await::fibers::self::Yield();
  }

  const await::executors::IExecutorPtr& Executor() {
    return GetRuntime().executor;
  }

  // Guids

  Guid GenerateGuid() const {
    return GetRuntime().guids->Generate();
  }

  // Cluster

  std::vector<std::string> DiscoverCluster() const {
    return GetRuntime().discovery->GetCluster();
  }

  // RPC

  const rpc::IServerPtr& RPCServer() const {
    return GetRuntime().rpc_server;
  }

  const rpc::IClientPtr& RPCClient() const {
    return GetRuntime().rpc_client;
  }

  // Local services

  const IGuidGeneratorPtr& GuidsGenerator() const {
    return GetRuntime().guids;
  }
};

}  // namespace whirl
