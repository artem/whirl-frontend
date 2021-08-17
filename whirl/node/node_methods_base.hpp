#pragma once

#include <whirl/node/runtime.hpp>

#include <await/fibers/core/api.hpp>
#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/future.hpp>

namespace whirl {

class NodeMethodsBase {
 protected:
  // Me

  NodeId Id() const {
    return GetRuntime().Config()->Id();
  }

  const std::string& HostName() const {
    return NetTransport()->HostName();
  }

  // Randomness

  IRandomService* RandomService() const {
    return GetRuntime().RandomService();
  }

  // [0, bound)
  size_t RandomNumber(size_t bound) const {
    return RandomService()->GenerateNumber(bound);
  }

  // [lo, hi)
  size_t RandomNumber(size_t lo, size_t hi) const {
    return lo + RandomNumber(hi - lo);
  }

  // Clocks

  ITimeService* TimeService() const {
    return GetRuntime().TimeService();
  }

  TimePoint WallTimeNow() const {
    return TimeService()->WallTimeNow();
  }

  TimePoint MonotonicNow() const {
    return TimeService()->MonotonicNow();
  }

  // Timeouts

  await::futures::Future<void> After(Duration d) {
    return TimeService()->After(d);
  }

  // TrueTime

  ITrueTimeService* TrueTime() const {
    return GetRuntime().TrueTime();
  }

  // Local storage backend

  node::db::IDatabase* Database() const {
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

  const await::executors::IExecutorPtr Executor() const {
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

  IGuidGenerator* GuidsGenerator() const {
    return GetRuntime().GuidGenerator();
  }

  node::fs::IFileSystem* FileSystem() const {
    return GetRuntime().FileSystem();
  }
};

}  // namespace whirl
