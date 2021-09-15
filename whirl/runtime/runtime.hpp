#pragma once

#include <await/executors/executor.hpp>
#include <await/fibers/core/manager.hpp>

#include <timber/logger.hpp>

#include <whirl/node/fs/fs.hpp>
#include <whirl/node/db/database.hpp>
#include <whirl/node/cluster/discovery.hpp>

#include <whirl/node/misc/config.hpp>
#include <whirl/node/time/time.hpp>
#include <whirl/node/random/random.hpp>
#include <whirl/node/misc/guid.hpp>
#include <whirl/node/time/true_time.hpp>
#include <whirl/node/net/transport.hpp>
#include <whirl/node/misc/terminal.hpp>
#include <whirl/node/misc/fault.hpp>

namespace whirl::node {

//////////////////////////////////////////////////////////////////////

struct IRuntime {
  virtual ~IRuntime() = default;

  // Execution

  virtual await::executors::IExecutor* Executor() = 0;

  virtual await::fibers::IFiberManager* FiberManager() = 0;

  // Time

  virtual time::ITimeService* TimeService() = 0;

  virtual time::ITrueTimeService* TrueTime() = 0;

  // Persistence

  virtual fs::IFileSystem* FileSystem() = 0;

  virtual db::IDatabase* Database() = 0;

  // Network

  virtual net::ITransport* NetTransport() = 0;

  virtual cluster::IDiscoveryService* DiscoveryService() = 0;

  // Logging

  virtual timber::ILogBackend* LogBackend() = 0;

  // Misc

  virtual IConfig* Config() = 0;

  virtual IRandomService* RandomService() = 0;

  virtual IGuidGenerator* GuidGenerator() = 0;

  virtual ITerminal* Terminal() = 0;
};

//////////////////////////////////////////////////////////////////////

// Bridge connecting engine-agnostic node and concrete engine
IRuntime& GetRuntime();

}  // namespace whirl::node
