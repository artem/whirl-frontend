#pragma once

#include <whirl/services/config.hpp>
#include <whirl/services/threads.hpp>
#include <whirl/services/time.hpp>
#include <whirl/services/local_storage.hpp>
#include <whirl/services/random.hpp>
#include <whirl/services/uid.hpp>
#include <whirl/services/true_time.hpp>
#include <whirl/services/filesystem.hpp>
#include <whirl/services/logger.hpp>
#include <whirl/services/discovery.hpp>

#include <whirl/rpc/impl/server.hpp>
#include <whirl/rpc/impl/client.hpp>

namespace whirl {

struct NodeServices {
  IConfigPtr config;

  IExecutorPtr executor;
  ITimeServicePtr time_service;

  IDiscoveryPtr discovery;

  rpc::IServerPtr rpc_server;
  rpc::IClientPtr rpc_client;

  // Use to create local storages
  ILocalStorageBackendPtr storage_backend;

  IRandomServicePtr random;
  IUidGeneratorPtr uids;
  ITrueTimeServicePtr true_time;

  INodeLoggerPtr logger;

  // TODO
  IFileSystemPtr fs;
};

}  // namespace whirl
