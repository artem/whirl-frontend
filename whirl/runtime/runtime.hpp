#pragma once

#include <await/executors/executor.hpp>
#include <await/fibers/core/manager.hpp>

#include <timber/logger.hpp>

#include <commute/transport/transport.hpp>

#include <whirl/node/time/time.hpp>
#include <whirl/node/time/true_time.hpp>

#include <whirl/node/fs/fs.hpp>
#include <whirl/node/db/database.hpp>

#include <whirl/node/cluster/discovery.hpp>

#include <whirl/node/random/service.hpp>
#include <whirl/node/guid/guid.hpp>
#include <whirl/node/config/config.hpp>
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

  // Net transport

  virtual commute::transport::ITransport* NetTransport() = 0;

  // Cluster

  virtual cluster::IDiscoveryService* DiscoveryService() = 0;

  // Logging

  virtual timber::ILogBackend* LoggerBackend() = 0;

  // Random

  virtual IRandomService* RandomService() = 0;

  // Guids

  virtual IGuidGenerator* GuidGenerator() = 0;

  // Configuration

  virtual IConfig* Config() = 0;

  // Misc

  virtual ITerminal* Terminal() = 0;
};

//////////////////////////////////////////////////////////////////////

// Bridge connecting engine-agnostic node and concrete engine
IRuntime& GetRuntime();

}  // namespace whirl::node
