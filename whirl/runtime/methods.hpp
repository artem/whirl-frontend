#pragma once

#include <whirl/runtime/runtime.hpp>

#include <whirl/rpc/client.hpp>
#include <whirl/rpc/server.hpp>

#include <await/fibers/core/api.hpp>
#include <await/fibers/core/await.hpp>
#include <await/fibers/sync/future.hpp>

#include <fmt/core.h>

namespace whirl::node::rt {

// Config

inline IConfig* Config() {
  return GetRuntime().Config();
}

inline NodeId Id() {
  return Config()->Id();
}

// Net

inline ITransportPtr NetTransport() {
  return GetRuntime().NetTransport();
}

inline const std::string& HostName() {
  return NetTransport()->HostName();
}

// File system

inline fs::IFileSystem* FileSystem() {
  return GetRuntime().FileSystem();
}

// Randomness

inline IRandomService* RandomService() {
  return GetRuntime().RandomService();
}

// [0, bound)
inline size_t RandomNumber(size_t bound) {
  return RandomService()->GenerateNumber(bound);
}

// [lo, hi)
inline size_t RandomNumber(size_t lo, size_t hi) {
  return lo + RandomNumber(hi - lo);
}

// Clocks

inline ITimeService* TimeService() {
  return GetRuntime().TimeService();
}

inline TimePoint WallTimeNow() {
  return TimeService()->WallTimeNow();
}

inline TimePoint MonotonicNow() {
  return TimeService()->MonotonicNow();
}

// Timeouts

inline await::futures::Future<void> After(Duration delay) {
  return TimeService()->After(delay);
}

// TrueTime

inline ITrueTimeService* TrueTime() {
  return GetRuntime().TrueTime();
}

// Database

inline db::IDatabase* Database() {
  return GetRuntime().Database();
}

// Execution

inline const await::executors::IExecutorPtr Executor() {
  return GetRuntime().Executor();
}

inline void Go(await::fibers::FiberRoutine routine) {
  await::fibers::Go(std::move(routine), Executor());
}

inline void SleepFor(Duration delay) {
  await::fibers::Await(After(delay)).ExpectOk();
}

inline void Yield() {
  await::fibers::self::Yield();
}

// DNS

inline IDiscoveryService* Dns() {
  return GetRuntime().DiscoveryService();
}

// Guids

inline IGuidGenerator* GuidsGenerator() {
  return GetRuntime().GuidGenerator();
}

inline Guid GenerateGuid() {
  return GetRuntime().GuidGenerator()->Generate();
}

// Terminal

inline ITerminal* Terminal() {
  return GetRuntime().Terminal();
}

template <typename ... Args>
inline void PrintLine(Args&& ... args) {
  std::string line = fmt::format(std::forward<Args>(args)...);
  Terminal()->PrintLine(line);
}

// RPC

rpc::IServerPtr MakeRpcServer();

rpc::IClientPtr MakeRpcClient();

}  // namespace whirl::node::rt
