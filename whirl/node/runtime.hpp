#pragma once

#include <whirl/services/config.hpp>
#include <whirl/services/time.hpp>
#include <whirl/services/database.hpp>
#include <whirl/services/random.hpp>
#include <whirl/services/guid.hpp>
#include <whirl/services/true_time.hpp>
#include <whirl/services/filesystem.hpp>
#include <whirl/services/discovery.hpp>
#include <whirl/services/net_transport.hpp>

#include <whirl/rpc/server.hpp>
#include <whirl/rpc/client.hpp>

namespace whirl {

//////////////////////////////////////////////////////////////////////

// Service locator

struct INodeRuntime {
  virtual ~INodeRuntime() = default;

  virtual IConfigPtr Config() = 0;

  virtual const IExecutorPtr& Executor() = 0;

  virtual ITimeServicePtr TimeService() = 0;

  virtual IDiscoveryPtr DiscoveryService() = 0;

  virtual ITransportPtr NetTransport() = 0;

  virtual IDatabasePtr Database() = 0;

  virtual IRandomServicePtr RandomService() = 0;
  virtual IGuidGeneratorPtr GuidGenerator() = 0;

  virtual ITrueTimeServicePtr TrueTime() = 0;

  virtual IFileSystemPtr FileSystem() = 0;
};

//////////////////////////////////////////////////////////////////////

// Bridge connecting engine-agnostic node and concrete engine
INodeRuntime& GetRuntime();

}  // namespace whirl
