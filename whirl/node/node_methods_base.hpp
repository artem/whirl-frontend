#pragma once

#include <whirl/node/services.hpp>
#include <whirl/node/threads.hpp>
#include <whirl/node/this.hpp>

#include <await/futures/helpers.hpp>

namespace whirl {

using await::futures::Future;

class NodeMethodsBase {
 protected:
  // Me

  NodeId Id() const {
    return ThisNodeServices().config->Id();
  }

  // Randomness

  const IRandomServicePtr& RandomService() const {
    return ThisNodeServices().random;
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
    return ThisNodeServices().time_service;
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
    return ThisNodeServices().true_time;
  }

  // Local storage backend

  const IDatabasePtr& StorageBackend() const {
    return ThisNodeServices().database;
  }

  void Spawn(ThreadRoutine routine) {
    await::fibers::Spawn(std::move(routine), Executor());
  }

  void SleepFor(Duration delay) {
    await::fibers::Await(After(delay)).ExpectOk();
  }

  void Yield() {
    await::fibers::self::Yield();
  }

  const await::executors::IExecutorPtr& Executor() {
    return ThisNodeServices().executor;
  }

  // Uids

  Uid GenerateUid() const {
    return ThisNodeServices().uids->Generate();
  }

  // Cluster

  std::vector<std::string> DiscoverCluster() const {
    return ThisNodeServices().discovery->GetCluster();
  }

  // RPC

  const rpc::IServerPtr& RPCServer() const {
    return ThisNodeServices().rpc_server;
  }

  const rpc::IClientPtr& RPCClient() const {
    return ThisNodeServices().rpc_client;
  }

  // Local services

  const IUidGeneratorPtr& UidsGenerator() const {
    return ThisNodeServices().uids;
  }

};

}  // namespace whirl
