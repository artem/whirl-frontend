#pragma once

#include <whirl/runtime/runtime.hpp>

// RPC
#include <whirl/node/rpc/client.hpp>
#include <whirl/node/rpc/server.hpp>

// Concurrency
#include <await/fibers/core/api.hpp>
#include <await/fibers/sync/future.hpp>

#include <timber/backend.hpp>

#include <fmt/core.h>

namespace whirl::node::rt {

// Config

inline IConfig* Config() {
  return GetRuntime().Config();
}

inline NodeId Id() {
  return Config()->Id();
}

inline std::string PoolName() {
  return Config()->PoolName();
}

// Net

inline net::ITransport* NetTransport() {
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

inline size_t RandomIndex(size_t size) {
  return RandomNumber(size);
}

// Clocks

inline time::ITimeService* TimeService() {
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

inline time::ITrueTimeService* TrueTime() {
  return GetRuntime().TrueTime();
}

// Database

inline db::IDatabase* Database() {
  return GetRuntime().Database();
}

// Execution

inline await::executors::IExecutor* Executor() {
  return GetRuntime().Executor();
}

inline await::fibers::IFiberManager* FiberManager() {
  return GetRuntime().FiberManager();
}

void Go(await::fibers::FiberRoutine routine);

inline void SleepFor(Duration delay) {
  await::fibers::Await(After(delay)).ExpectOk();
}

// DNS

inline cluster::IDiscoveryService* Discovery() {
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

template <typename FormatString, typename... Args>
inline void PrintLine(FormatString&& format_string, Args&&... args) {
  std::string line =
      fmt::format(fmt::runtime(std::forward<FormatString>(format_string)),
                  std::forward<Args>(args)...);
  Terminal()->PrintLine(line);
}

// Logging

inline timber::ILogBackend* LogBackend() {
  return GetRuntime().LogBackend();
}

// RPC

rpc::IServerPtr MakeRpcServer();

rpc::IClientPtr MakeRpcClient();

}  // namespace whirl::node::rt
