#pragma once

#include <await/executors/executor.hpp>
#include <await/fibers/core/manager.hpp>

#include <timber/logger.hpp>

#include <commute/transport/transport.hpp>

#include <persist/fs/fs.hpp>

#include <whirl/node/time/time_service.hpp>
#include <whirl/node/time/true_time_service.hpp>

#include <whirl/node/db/database.hpp>

#include <whirl/node/cluster/discovery.hpp>

#include <whirl/node/random/service.hpp>
#include <whirl/node/guids/service.hpp>
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

  virtual persist::fs::IFileSystem* FileSystem() = 0;

  virtual db::IDatabase* Database() = 0;

  // Net transport

  virtual commute::transport::ITransport* NetTransport() = 0;

  // Cluster

  virtual cluster::IDiscoveryService* DiscoveryService() = 0;

  // Logging

  virtual timber::ILogBackend* LoggerBackend() = 0;

  // Random

  virtual random::IRandomService* RandomService() = 0;

  // Guids

  virtual guids::IGuidGenerator* GuidGenerator() = 0;

  // Configuration

  virtual cfg::IConfig* Config() = 0;

  // Misc

  virtual ITerminal* Terminal() = 0;
};

}  // namespace whirl::node
