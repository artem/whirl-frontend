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
    return GetRuntime().Config()->Id();
  }

  // Randomness

  IRandomServicePtr RandomService() const {
    return GetRuntime().RandomService();
  }

  size_t RandomNumber() const {
    return RandomService()->GenerateNumber();
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

  ITimeServicePtr TimeService() const {
    return GetRuntime().TimeService();
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

  ITrueTimeServicePtr TrueTime() const {
    return GetRuntime().TrueTime();
  }

  // Local storage backend

  IDatabasePtr StorageBackend() const {
    return GetRuntime().Database();
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

  const await::executors::IExecutorPtr Executor() {
    return GetRuntime().Executor();
  }

  // Guids

  Guid GenerateGuid() const {
    return GetRuntime().GuidGenerator()->Generate();
  }

  // Cluster

  std::vector<std::string> DiscoverCluster() const {
    return GetRuntime().DiscoveryService()->GetCluster();
  }

  // RPC

  ITransportPtr NetTransport() const {
    return GetRuntime().NetTransport();
  }

  // Local services

  IGuidGeneratorPtr GuidsGenerator() const {
    return GetRuntime().GuidGenerator();
  }
};

}  // namespace whirl
