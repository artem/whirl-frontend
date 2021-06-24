#include <whirl/engines/matrix/server/server.hpp>

#include <whirl/engines/matrix/memory/helpers.hpp>

#include <whirl/engines/matrix/process/crash.hpp>

// Services

#include <whirl/engines/matrix/server/services/config.hpp>
#include <whirl/engines/matrix/server/services/executor.hpp>
#include <whirl/engines/matrix/server/services/time.hpp>
#include <whirl/engines/matrix/server/services/database.hpp>
#include <whirl/engines/matrix/server/services/true_time.hpp>
#include <whirl/engines/matrix/server/services/uid.hpp>
#include <whirl/engines/matrix/server/services/random.hpp>
#include <whirl/engines/matrix/server/services/net_transport.hpp>
#include <whirl/engines/matrix/server/services/discovery.hpp>

#include <whirl/rpc/server_impl.hpp>

#include <whirl/helpers/digest.hpp>

namespace whirl {

Server::Server(net::Network& net, ServerConfig config, INodeFactoryPtr factory)
    : config_(config),
      node_factory_(std::move(factory)),
      transport_(net, config.hostname, heap_) {
}

Server::~Server() {
  node_factory_.reset();
  WHEELS_VERIFY(state_ == State::Crashed, "Invalid state");
}

// INetServer

void Server::HandlePacket(const net::Packet& packet, net::Link* out) {
  transport_.HandlePacket(packet, out);
}

// IFaultyServer

bool Server::IsAlive() const {
  return state_ == State::Running || state_ == State::Paused;
}

void Server::Crash() {
  GlobalAllocatorGuard g;

  WHEELS_VERIFY(state_ != State::Crashed, "Server already crashed");

  WHIRL_LOG("Crash server {}", HostName());

  // 1) Remove all network endpoints
  transport_.Reset();

  // 2) Reset process heap
  // WHIRL_LOG("Bytes allocated on process heap: " << heap_.BytesAllocated());
  {
    auto g = heap_.Use();
    steps_ = nullptr;
    ReleaseFiberResourcesOnCrash(heap_);
  }
  heap_.Reset();

  state_ = State::Crashed;
}

void Server::FastReboot() {
  GlobalAllocatorGuard g;

  Crash();
  Start();
}

void Server::Pause() {
  GlobalAllocatorGuard g;

  WHEELS_VERIFY(state_ != State::Paused, "Server already paused");
  state_ = State::Paused;
}

void Server::Resume() {
  GlobalAllocatorGuard g;

  WHEELS_VERIFY(state_ == State::Paused, "Server is not paused");

  auto now = GlobalNow();

  {
    auto g = heap_.Use();

    while (!steps_->IsEmpty() && steps_->NextStepTime() < now) {
      auto step = steps_->TakeNext();
      steps_->Add(now, std::move(step.action));
    }
  }

  state_ = State::Running;
}

void Server::AdjustWallClock() {
  GlobalAllocatorGuard g;
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

  WHIRL_LOG("Start node at server {}", HostName());

  auto g = heap_.Use();

  steps_ = new StepQueue();

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
  // No [de]allocations here
  // auto g = heap_.Use();
  // WHEELS_VERIFY(steps_, "Step queue is not created");
  return !steps_->IsEmpty();
}

TimePoint Server::NextStepTime() const {
  // No [de]allocations here
  // auto g = heap_.Use();
  return steps_->NextStepTime();
}

void Server::Step() {
  auto g = heap_.Use();
  auto step = steps_->TakeNext();
  step();
}

void Server::Shutdown() {
  if (state_ != State::Crashed) {
    Crash();
  }
}

size_t Server::ComputeDigest() const {
  if (state_ == State::Crashed) {
    return 0;
  }

  DigestCalculator digest;
  digest.Eat(heap_.BytesAllocated());
  digest.Eat(persistent_storage_.ComputeDigest());
  return digest.Get();
}

// Private

NodeServices Server::CreateNodeServices() {
  NodeServices services;

  services.config = std::make_shared<Config>(config_.id);

  auto executor = std::make_shared<EventQueueExecutor>(*steps_);
  auto time_service =
      std::make_shared<TimeService>(wall_clock_, monotonic_clock_, *steps_);

  services.executor = executor;
  services.time_service = time_service;

  services.database = std::make_shared<Database>(persistent_storage_);

  static const net::Port kTransportPort = 42;
  auto net_transport =
      std::make_shared<NetTransport>(transport_, kTransportPort);

  services.discovery = std::make_shared<DiscoveryService>();

  services.rpc_server =
      std::make_shared<rpc::ServerImpl>(net_transport, executor);
  services.rpc_client = rpc::MakeClient(net_transport, executor);

  services.random = std::make_shared<RandomService>();
  services.uids = std::make_shared<UidGenerator>(config_.id);
  services.true_time = std::make_shared<TrueTimeService>();

  return services;
}

}  // namespace whirl