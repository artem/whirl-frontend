#pragma once

#include <whirl/runtime/runtime.hpp>

// Runtime impl

#include <whirl/engines/matrix/server/runtime/random.hpp>
#include <whirl/engines/matrix/server/runtime/thread_pool.hpp>
#include <whirl/engines/matrix/server/runtime/time.hpp>
#include <whirl/engines/matrix/server/runtime/filesystem.hpp>
#include <whirl/engines/matrix/server/runtime/true_time.hpp>
#include <whirl/engines/matrix/server/runtime/guid.hpp>
#include <whirl/engines/matrix/server/runtime/net_transport.hpp>
#include <whirl/engines/matrix/server/runtime/database.hpp>
#include <whirl/engines/matrix/server/runtime/discovery.hpp>
#include <whirl/engines/matrix/server/runtime/config.hpp>
#include <whirl/engines/matrix/server/runtime/terminal.hpp>

#include <whirl/engines/matrix/process/fibers.hpp>

#include <optional>

//////////////////////////////////////////////////////////////////////

namespace whirl::matrix {

template <typename T>
class StaticObject {
 public:
  template <typename... Args>
  void Init(Args&&... args) {
    object_.emplace(std::forward<Args>(args)...);
  }

  T* Get() {
    return &*object_;
  }

  T* operator->() {
    return Get();
  }

 private:
  std::optional<T> object_;
};

//////////////////////////////////////////////////////////////////////

struct NodeRuntime : node::IRuntime {
  // Objects

  StaticObject<matrix::ThreadPool> thread_pool;
  StaticObject<matrix::process::FiberManager> fibers;
  StaticObject<matrix::TimeService> time;
  StaticObject<matrix::FS> fs;
  StaticObject<matrix::NetTransport> transport;
  StaticObject<matrix::db::Database> db;
  StaticObject<matrix::RandomGenerator> random;
  StaticObject<matrix::GuidGenerator> guids;
  StaticObject<matrix::TrueTimeService> true_time;
  StaticObject<matrix::DiscoveryService> discovery;
  StaticObject<matrix::Config> config;
  StaticObject<matrix::Terminal> terminal;

  // Accessors

  const await::executors::IExecutorPtr& Executor() {
    return thread_pool->GetExecutor();
  }

  await::fibers::IFiberManager* FiberManager() {
    return fibers.Get();
  }

  ITimeService* TimeService() {
    return time.Get();
  }

  ITransport* NetTransport() {
    return transport.Get();
  }

  node::db::IDatabase* Database() {
    return db.Get();
  }

  IGuidGenerator* GuidGenerator() {
    return guids.Get();
  }

  IRandomService* RandomService() {
    return random.Get();
  }

  ITrueTimeService* TrueTime() {
    return true_time.Get();
  }

  node::fs::IFileSystem* FileSystem() {
    return fs.Get();
  }

  IConfig* Config() {
    return config.Get();
  }

  IDiscoveryService* DiscoveryService() {
    return discovery.Get();
  }

  ITerminal* Terminal() {
    return terminal.Get();
  }
};

}  // namespace whirl::matrix
