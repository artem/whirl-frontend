#pragma once

#include <whirl/services/threads.hpp>
#include <whirl/services/time.hpp>
#include <whirl/services/local_storage.hpp>
#include <whirl/services/random.hpp>
#include <whirl/services/uid.hpp>
#include <whirl/services/true_time.hpp>
#include <whirl/services/filesystem.hpp>
#include <whirl/services/logger.hpp>

#include <whirl/rpc/use/server.hpp>
#include <whirl/rpc/use/client.hpp>

namespace whirl {

struct NodeServices {
  ThreadsRuntime threads;
  ITimeServicePtr time_service;

  rpc::TRPCServer rpc_server;
  rpc::TRPCClient rpc_client;

  LocalStorage local_storage;
  // Use to create typed local storages
  ILocalStorageBackendPtr storage_engine;

  IRandomServicePtr random;
  IUidGeneratorPtr uids;
  ITrueTimeServicePtr true_time;

  INodeLoggerPtr logger;

  // TODO
  IFileSystemPtr fs;
};

}  // namespace whirl
