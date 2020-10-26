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
#include <whirl/matrix/server/services/discovery.hpp>

#include <whirl/rpc/impl/server_impl.hpp>

namespace whirl {

NodeServices Server::CreateNodeServices() {
  NodeServices services;

  auto executor = std::make_shared<EventQueueExecutor>(events_);
  auto time_service =
      std::make_shared<TimeService>(wall_clock_, monotonic_clock_, events_);

  services.threads = ThreadsRuntime{executor, time_service};
  services.time_service = time_service;

  services.storage_backend = std::make_shared<LocalStorageBackend>(storage_);

  auto net_transport = std::make_shared<NetTransport>(heap_, network_);

  services.discovery = std::make_shared<DiscoveryService>();

  services.rpc_server =
      std::make_shared<rpc::ServerImpl>(net_transport, executor);
  services.rpc_client = rpc::TClient(net_transport, executor);

  services.random = std::make_shared<RandomService>();
  services.uids = std::make_shared<UidGenerator>();
  services.true_time = std::make_shared<TrueTimeService>();

  services.logger = std::make_shared<LoggerProxy>();

  return services;
}

}  // namespace whirl