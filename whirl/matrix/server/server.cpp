#include <whirl/matrix/server/server.hpp>

// Services

#include <whirl/matrix/server/services/executor.hpp>
#include <whirl/matrix/server/services/time.hpp>
#include <whirl/matrix/server/services/local_storage.hpp>
#include <whirl/matrix/server/services/true_time.hpp>
#include <whirl/matrix/server/services/uid.hpp>
#include <whirl/matrix/server/services/random.hpp>
#include <whirl/matrix/server/services/net_transport.hpp>
#include <whirl/matrix/server/services/logger.hpp>

namespace whirl {

NodeServices Server::CreateNodeServices() {
  NodeServices services;

  auto executor = std::make_shared<EventQueueExecutor>(events_);
  auto time_service = std::make_shared<TimeService>(
      wall_time_clock_, monotonic_clock_, events_);

  services.threads = ThreadsRuntime{executor, time_service};
  services.time_service = time_service;

  services.storage_engine = std::make_shared<LocalStorageEngine>(storage_);
  services.local_storage = LocalStorage(services.storage_engine);

  auto net_transport = std::make_shared<NetTransport>(heap_, network_);

  services.rpc_server = rpc::TRPCServer(net_transport, executor);
  services.rpc_client = rpc::TRPCClient(net_transport, executor);

  services.random = std::make_shared<RandomService>();
  services.uids = std::make_shared<UidGenerator>();
  services.true_time = std::make_shared<TrueTimeService>();

  services.logger = std::make_shared<LoggerProxy>();

  return services;
}

}  // namespace whirl