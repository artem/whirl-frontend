#pragma once

#include <whirl/runtime/access.hpp>

// RPC
#include <commute/rpc/client.hpp>
#include <commute/rpc/server.hpp>

// Concurrency
#include <await/fibers/core/api.hpp>
#include <await/fibers/sync/future.hpp>

// Logging
#include <timber/backend.hpp>

#include <fmt/core.h>

namespace whirl::node::rt {

// Shortcuts for runtime services

// Net

inline commute::transport::ITransport* NetTransport() {
  return GetRuntime().NetTransport();
}

inline const std::string& HostName() {
  return NetTransport()->HostName();
}

// Filesystem

inline persist::fs::IFileSystem* FileSystem() {
  return GetRuntime().FileSystem();
}

inline persist::fs::Path MakeFsPath(const std::string& repr) {
  return FileSystem()->MakePath(repr);
}

inline persist::fs::Path FsRootPath() {
  return FileSystem()->RootPath();
}

inline persist::fs::Path FsTmpPath() {
  return FileSystem()->TmpPath();
}

// Randomness

inline random::IRandomService* RandomService() {
  return GetRuntime().RandomService();
}

// [0, bound)
inline size_t RandomNumber(size_t bound) {
  return RandomService()->GenerateNumber(bound);
}

// [lo, hi]
uint64_t RandomNumber(uint64_t lo, uint64_t hi);

// [0, size)
size_t RandomIndex(size_t size);

// Clocks

inline time::ITimeService* TimeService() {
  return GetRuntime().TimeService();
}

inline time::WallTime WallTimeNow() {
  return TimeService()->WallTimeNow();
}

inline time::MonotonicTime MonotonicNow() {
  return TimeService()->MonotonicNow();
}

// Timeouts

inline await::futures::Future<void> After(Jiffies delay) {
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

inline void SleepFor(Jiffies delay) {
  await::fibers::Await(After(delay)).ExpectOk();
}

// Discovery

inline cluster::IDiscoveryService* Discovery() {
  return GetRuntime().DiscoveryService();
}

// Guids

inline guids::IGuidGenerator* GuidGenerator() {
  return GetRuntime().GuidGenerator();
}

inline std::string GenerateGuid() {
  return GetRuntime().GuidGenerator()->Generate();
}

// Config

inline cfg::IConfig* Config() {
  return GetRuntime().Config();
}

// Logging

inline timber::ILogBackend* LoggerBackend() {
  return GetRuntime().LoggerBackend();
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

}  // namespace whirl::node::rt
