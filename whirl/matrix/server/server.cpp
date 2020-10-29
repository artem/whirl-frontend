#include <whirl/matrix/server/server.hpp>

#include <whirl/matrix/common/hide_to_heap.hpp>
#include <whirl/matrix/common/copy.hpp>

// Services

#include <whirl/matrix/server/services/config.hpp>
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

Server::Server(Network& network, ServerConfig config, INodeFactoryPtr factory)
    : config_(config),
      node_factory_(std::move(factory)),
      network_(network, Name()) {
}

Server::~Server() {
  node_factory_.reset();
  WHEELS_VERIFY(state_ == State::Crashed, "Invalid state");
}

// IFaultyServer

void Server::Crash() {
  GlobalHeapScope g;

  WHIRL_LOG("Crash server " << Name());

  // Reset all client connections
  network_.Reset();

  WHIRL_LOG("Bytes allocated on process heap: " << heap_.BytesAllocated());
  {
    auto g = heap_.Use();
    events_.Clear();
  }
  heap_.Reset();

  paused_ = false;

  state_ = State::Crashed;
}

void Server::Reboot() {
  GlobalHeapScope g;

  Crash();
  Start();
}

void Server::Pause() {
  GlobalHeapScope g;

  WHEELS_VERIFY(!paused_, "Server already paused");
  paused_ = true;
  state_ = State::Paused;
}

void Server::Resume() {
  GlobalHeapScope g;

  WHEELS_VERIFY(paused_, "Server is not paused");

  auto now = GlobalNow();

  {
    auto g = heap_.Use();

    while (!events_.IsEmpty() && events_.NextEventTime() < now) {
      auto pending_event = events_.TakeNext();
      events_.Add(now, std::move(pending_event.action));
    }
  }

  paused_ = false;
  state_ = State::Running;
}

void Server::AdjustWallClock() {
  GlobalHeapScope g;
  wall_clock_.AdjustOffset();
}

// IActor

void Server::Start() {
  monotonic_clock_.Reset();

  WHIRL_LOG("Start node at server " << Name());

  auto g = heap_.Use();

  auto services = CreateNodeServices();
  auto node = node_factory_->CreateNode(std::move(services));
  node->Start();
  HideToHeap(std::move(node));

  state_ = State::Running;
}

bool Server::IsRunnable() const {
  if (paused_) {
    return false;
  }
  auto g = heap_.Use();
  return !events_.IsEmpty();
}

TimePoint Server::NextStepTime() {
  auto g = heap_.Use();
  return events_.NextEventTime();
}

void Server::Step() {
  auto g = heap_.Use();
  auto event = events_.TakeNext();
  event();
}

void Server::Shutdown() {
  Crash();
}

// Private

NodeServices Server::CreateNodeServices() {
  NodeServices services;

  services.config = std::make_shared<Config>(config_.id);

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