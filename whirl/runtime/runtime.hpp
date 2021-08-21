#pragma once

#include <whirl/fs/fs.hpp>
#include <whirl/db/database.hpp>

#include <whirl/services/config.hpp>
#include <whirl/services/time.hpp>
#include <whirl/services/random.hpp>
#include <whirl/services/guid.hpp>
#include <whirl/services/true_time.hpp>
#include <whirl/services/discovery.hpp>
#include <whirl/services/net_transport.hpp>
#include <whirl/services/terminal.hpp>
#include <whirl/services/fault.hpp>

#include <await/fibers/core/manager.hpp>

namespace whirl::node {

//////////////////////////////////////////////////////////////////////

// Service locator

struct IRuntime {
  virtual ~IRuntime() = default;

  virtual IConfig* Config() = 0;

  virtual const await::executors::IExecutorPtr& Executor() = 0;

  virtual await::fibers::IFiberManager* FiberManager() = 0;

  virtual ITimeService* TimeService() = 0;

  virtual IDiscoveryService* DiscoveryService() = 0;

  virtual ITransport* NetTransport() = 0;

  virtual db::IDatabase* Database() = 0;

  virtual IRandomService* RandomService() = 0;
  virtual IGuidGenerator* GuidGenerator() = 0;

  virtual ITrueTimeService* TrueTime() = 0;

  virtual fs::IFileSystem* FileSystem() = 0;

  virtual ITerminal* Terminal() = 0;
};

//////////////////////////////////////////////////////////////////////

// Bridge connecting engine-agnostic node and concrete engine
IRuntime& GetRuntime();

}  // namespace whirl::node
