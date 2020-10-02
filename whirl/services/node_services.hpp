#pragma once

// Node services

#include <whirl/services/executor.hpp>
#include <whirl/matrix/runtime.hpp>
#include <whirl/services/time_service.hpp>
#include <whirl/services/local_storage.hpp>
//#include <whirl/services/message_bus.hpp>
#include <whirl/services/filesystem.hpp>
#include <whirl/services/true_time.hpp>
#include <whirl/services/rpc.hpp>
#include <whirl/services/random.hpp>
#include <whirl/services/uid.hpp>

namespace whirl {

struct NodeServices {
  Runtime runtime;
  ITimeServicePtr time_service;

  TRPCServer rpc_server;
  TRPCClient rpc_client;

  LocalStorage local_storage;
  // Use to create typed local storages
  ILocalStorageEnginePtr storage_engine;

  IRandomServicePtr random;
  IUidGeneratorPtr uids_;
  ITrueTimeServicePtr true_time;

  // TODO
  IFileSystemPtr fs;
};

}  // namespace whirl
