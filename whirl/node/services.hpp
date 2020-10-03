#pragma once

#include <whirl/services/threads.hpp>
#include <whirl/services/time.hpp>
#include <whirl/services/local_storage.hpp>
#include <whirl/services/rpc.hpp>
#include <whirl/services/random.hpp>
#include <whirl/services/uid.hpp>
#include <whirl/services/true_time.hpp>
#include <whirl/services/filesystem.hpp>

namespace whirl {

struct NodeServices {
  ThreadsRuntime threads;
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
