#pragma once

// Runtime impl

#include <whirl/engines/matrix/server/services/random.hpp>
#include <whirl/engines/matrix/server/services/thread_pool.hpp>
#include <whirl/engines/matrix/server/services/time.hpp>
#include <whirl/engines/matrix/server/services/filesystem.hpp>
#include <whirl/engines/matrix/server/services/true_time.hpp>
#include <whirl/engines/matrix/server/services/guid.hpp>
#include <whirl/engines/matrix/server/services/net_transport.hpp>
#include <whirl/engines/matrix/server/services/database.hpp>
#include <whirl/engines/matrix/server/services/discovery.hpp>
#include <whirl/engines/matrix/server/services/config.hpp>
#include <whirl/engines/matrix/server/services/terminal.hpp>

#include <optional>

//////////////////////////////////////////////////////////////////////

namespace whirl::matrix {

template <typename T>
class StaticObject {
 public:
  template <typename ... Args>
  void Init(Args&& ... args) {
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

struct NodeRuntime {
  StaticObject<ThreadPool> thread_pool;
  StaticObject<TimeService> time;
  StaticObject<FS> fs;
  StaticObject<NetTransport> transport;
  StaticObject<db::Database> db;
  StaticObject<RandomGenerator> random;
  StaticObject<GuidGenerator> guids;
  StaticObject<TrueTimeService> true_time;
  StaticObject<Terminal> terminal;

  // TODO
  StaticObject<Config> config;
  // TODO
  StaticObject<DiscoveryService> discovery;
};

}  // namespace whirl::matrix