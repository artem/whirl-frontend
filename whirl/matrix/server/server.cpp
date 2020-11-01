#include <whirl/matrix/server/server.hpp>

#include <whirl/matrix/common/hide_to_heap.hpp>
#include <whirl/matrix/common/copy.hpp>
#include <whirl/matrix/process/crash.hpp>

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

Server::Server(net::Network& net, ServerConfig config, INodeFactoryPtr factory)
    : config_(config),
      node_factory_(std::move(factory)),
      transport_(net, Name(), heap_) {
}

Server::~Server() {
  node_factory_.reset();
  WHEELS_VERIFY(state_ == State::Crashed, "Invalid state");
}

// INetServer

void Server::HandlePacket(const net::Packet& packet, net::Link* out) {
  auto g = heap_.Use();

  // Switch to server actor just for better logging
  auto handle_packet = [this, packet, out]() {
    GlobalHeapScope g;
    transport_.HandlePacket(packet, out);
  };
  events_->Add(GlobalNow(), std::move(handle_packet));
}

// IFaultyServer

bool Server::IsAlive() const {
  return state_ == State::Running || state_ == State::Paused;
}

void Server::Crash() {
  GlobalHeapScope g;

  WHEELS_VERIFY(state_ != State::Crashed, "Server already crashed");

  WHIRL_LOG("Crash server " << HostName());

  // 1) Remove all network endpoints
  transport_.Reset();

  // 2) Reset process heap
  // WHIRL_LOG("Bytes allocated on process heap: " << heap_.BytesAllocated());
  {
    auto g = heap_.Use();
    events_ = nullptr;
    ReleaseFibersOnCrash(heap_);
  }
  heap_.Reset();

  state_ = State::Crashed;
}

void Server::FastReboot() {
  GlobalHeapScope g;

  Crash();
  Start();
}

void Server::Pause() {
  GlobalHeapScope g;

  WHEELS_VERIFY(state_ != State::Paused, "Server already paused");
  state_ = State::Paused;
}

void Server::Resume() {
  GlobalHeapScope g;

  WHEELS_VERIFY(state_ == State::Paused, "Server is not paused");

  auto now = GlobalNow();

  {
    auto g = heap_.Use();

    while (!events_->IsEmpty() && events_->NextEventTime() < now) {
      auto pending_event = events_->TakeNext();
      events_->Add(now, std::move(pending_event.action));
    }
  }

  state_ = State::Running;
}

void Server::AdjustWallClock() {
  GlobalHeapScope g;
  wall_clock_.AdjustOffset();
}

// IActor

const std::string& Server::Name() const {
  return HostName();
}

void Server::Start() {
  WHEELS_VERIFY(state_ == State::Initial || state_ == State::Crashed,
                "Invalid state");

  monotonic_clock_.Reset();

  WHIRL_LOG("Start node at server " << Name());

  auto g = heap_.Use();

  // events_ = new EventQueue() ?
  events_ = heap_.New<EventQueue>();

  auto services = CreateNodeServices();
  auto node = node_factory_->CreateNode(std::move(services));
  node->Start();
  HideToHeap(std::move(node));

  state_ = State::Running;
}

bool Server::IsRunnable() const {
  if (state_ != State::Running) {
    return false;
  }
  auto g = heap_.Use();
  WHEELS_VERIFY(events_, "Event queue is not created");
  return !events_->IsEmpty();
}

TimePoint Server::NextStepTime() {
  auto g = heap_.Use();
  return events_->NextEventTime();
}

void Server::Step() {
  auto g = heap_.Use();
  auto event = events_->TakeNext();
  event();
}

void Server::Shutdown() {
  if (state_ != State::Crashed) {
    Crash();
  }
}

// Private

NodeServices Server::CreateNodeServices() {
  NodeServices services;

  services.config = std::make_shared<Config>(config_.id);

  auto executor = std::make_shared<EventQueueExecutor>(*events_);
  auto time_service =
      std::make_shared<TimeService>(wall_clock_, monotonic_clock_, *events_);

  services.threads = ThreadsRuntime{executor, time_service};
  services.time_service = time_service;

  services.storage_backend =
      std::make_shared<LocalStorageBackend>(persistent_storage_);

  auto net_transport = std::make_shared<NetTransport>(transport_);

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